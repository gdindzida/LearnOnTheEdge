#include "blur_kernel.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>

#ifdef IMAGE_BLUR_FASTRPC
#include "image_blur.h"
#include "remote.h"
#include "rpcmem.h"
#endif

namespace {
constexpr int width = 100;
constexpr int height = 100;
constexpr int pixels = width * height;
using Image = std::array<uint8_t, pixels>;
using Clock = std::chrono::steady_clock;

struct Options {
    std::string backend;
    uint32_t seed = 42;
    std::filesystem::path output_dir = "output";
};

void usage()
{
    std::cout << "Usage: image_blur_app --backend cpu|dsp [--seed UINT32] "
                 "[--output-dir PATH]\n"
                 "Generate a 100x100 grayscale image and apply a 3x3 box blur.\n"
                 "Defaults: seed=42, output-dir=output. DSP uses unsigned CDSP.\n";
}

Options parse_options(int argc, char **argv)
{
    Options options;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg != "--backend" && arg != "--seed" && arg != "--output-dir") {
            throw std::runtime_error("Unknown option: " + arg);
        }
        if (++i == argc) throw std::runtime_error("Missing value for " + arg);
        const std::string value = argv[i];
        if (arg == "--backend") {
            options.backend = value;
        } else if (arg == "--seed") {
            if (value.empty() || value.find_first_not_of("0123456789") != std::string::npos) {
                throw std::runtime_error("Seed must be an integer in [0, 4294967295].");
            }
            const auto seed = std::stoull(value);
            if (seed > std::numeric_limits<uint32_t>::max()) {
                throw std::runtime_error("Seed exceeds 4294967295.");
            }
            options.seed = static_cast<uint32_t>(seed);
        } else {
            if (value.empty()) throw std::runtime_error("Output directory must not be empty.");
            options.output_dir = value;
        }
    }
    if (options.backend != "cpu" && options.backend != "dsp") {
        throw std::runtime_error("Specify --backend cpu or --backend dsp.");
    }
    return options;
}

void cpu_blur(const Image &input, Image &output)
{
    if (blur_box3x3(input.data(), pixels, output.data(), pixels, width, height) != 0) {
        throw std::runtime_error("CPU blur rejected the image buffers.");
    }
}

double elapsed_us(Clock::time_point start)
{
    return std::chrono::duration<double, std::micro>(Clock::now() - start).count();
}

#ifdef IMAGE_BLUR_FASTRPC
void check_rpc(int result, const char *operation)
{
    if (result != 0) {
        throw std::runtime_error(std::string(operation) + " failed (FastRPC status " +
                                 std::to_string(result) + ").");
    }
}

struct DspSession {
    remote_handle64 handle = 0;
    DspSession()
    {
        remote_rpc_control_unsigned_module control{};
        control.domain = CDSP_DOMAIN_ID;
        control.enable = 1;
        check_rpc(remote_session_control(DSPRPC_CONTROL_UNSIGNED_MODULE,
                                         &control, sizeof(control)), "Request unsigned CDSP");
        const std::string uri = std::string(image_blur_URI) + CDSP_DOMAIN;
        check_rpc(image_blur_open(uri.c_str(), &handle), "Open image_blur DSP library");
    }
    ~DspSession() { image_blur_close(handle); }
    DspSession(const DspSession &) = delete;
    DspSession &operator=(const DspSession &) = delete;
};

struct RpcMemDeleter {
    void operator()(uint8_t *ptr) const { rpcmem_free(ptr); }
};
using RpcBuffer = std::unique_ptr<uint8_t, RpcMemDeleter>;

// Returns wall time for the RPC call, including transport but not session setup.
double dsp_blur(const Image &input, Image &output)
{
    DspSession session;
    RpcBuffer shared_input(static_cast<uint8_t *>(
        rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS, pixels)));
    RpcBuffer shared_output(static_cast<uint8_t *>(
        rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS, pixels)));
    if (!shared_input || !shared_output) throw std::runtime_error("rpcmem_alloc failed.");
    std::copy(input.begin(), input.end(), shared_input.get());
    const auto start = Clock::now();
    const int result = image_blur_apply(session.handle, shared_input.get(), pixels,
                                       shared_output.get(), pixels, width, height);
    const double duration = elapsed_us(start);
    check_rpc(result, "DSP blur");
    std::copy_n(shared_output.get(), pixels, output.begin());
    return duration;
}
#endif

void write_pgm(const std::filesystem::path &path, const Image &image)
{
    std::ofstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open image file: " + path.string());
    file << "P5\n" << width << ' ' << height << "\n255\n";
    file.write(reinterpret_cast<const char *>(image.data()), image.size());
    file.close();
    if (!file) throw std::runtime_error("Cannot write image file: " + path.string());
}
} // namespace

int main(int argc, char **argv)
{
    try {
        if (argc == 2 && std::string(argv[1]) == "--help") {
            usage();
            return 0;
        }
        const Options options = parse_options(argc, argv);
#ifndef IMAGE_BLUR_FASTRPC
        if (options.backend == "dsp") {
            throw std::runtime_error("This is a CPU-only build. Build with the SDK's "
                                     "ubuntuARM toolchain and run on the board for DSP support.");
        }
#endif
        Image input{}, output{};
        std::mt19937 random(options.seed);
        for (auto &pixel : input) pixel = static_cast<uint8_t>(random() & 0xffu);

        double duration = 0;
        if (options.backend == "cpu") {
            const auto start = Clock::now();
            cpu_blur(input, output);
            duration = elapsed_us(start);
        } else {
#ifdef IMAGE_BLUR_FASTRPC
            duration = dsp_blur(input, output);
            Image reference{};
            cpu_blur(input, reference);
            const auto mismatch = std::mismatch(output.begin(), output.end(), reference.begin());
            if (mismatch.first != output.end()) {
                throw std::runtime_error("DSP/CPU mismatch at pixel " +
                    std::to_string(mismatch.first - output.begin()));
            }
            std::cout << "Verification: all 10000 DSP pixels match CPU output.\n";
#endif
        }

        std::filesystem::create_directories(options.output_dir);
        write_pgm(options.output_dir / "input.pgm", input);
        write_pgm(options.output_dir / "blurred.pgm", output);
        std::cout << "Backend: " << options.backend << "\nImage: 100x100 grayscale, seed="
                  << options.seed << "\nFilter: 3x3 box, clamped edges, integer division\n"
                  << (options.backend == "dsp" ? "DSP RPC time (includes transport): "
                                                : "CPU kernel time: ")
                  << duration << " us\nImages: " << options.output_dir.string() << '\n';
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "image_blur_app: " << error.what() << '\n';
        return 1;
    }
}
