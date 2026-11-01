# tmux keybindings

## Navigation

- [Home](../../README.md)
- [Parent: tmux configuration](README.md)

```tmux
# ── Prefix ────────────────────────────────────────────────────────────────────
# Change prefix from Ctrl+b to Ctrl+a (easier to reach)
unbind C-b
set -g prefix C-space
bind C-space send-prefix

# ── Reload config ─────────────────────────────────────────────────────────────
bind r source-file ~/.tmux.conf \; display "Config reloaded!"

#── Splits ────────────────────────────────────────────────────────────────────
# Use | and - for intuitive splits (instead of % and ")
bind | split-window -h -c "#{pane_current_path}"
bind - split-window -v -c "#{pane_current_path}"
unbind '"'
unbind %

# ── Pane navigation (Vim-style) ────────────────────────────────────────────────
bind h select-pane -L
bind j select-pane -D
bind k select-pane -U
bind l select-pane -R

# ── Pane resizing ─────────────────────────────────────────────────────────────
bind -r H resize-pane -L 5
bind -r J resize-pane -D 5
bind -r K resize-pane -U 5
bind -r L resize-pane -R 5

# ── Window navigation ─────────────────────────────────────────────────────────
bind -r C-h previous-window # Ctrl+a, Ctrl+h
bind -r C-l next-window # Ctrl+a, Ctrl+l
bind Tab last-window # quick toggle between last two windows

# ── Copy mode (Vim-style) ─────────────────────────────────────────────────────
setw -g mode-keys vi
bind Enter copy-mode
bind -T copy-mode-vi v send -X begin-selection
bind -T copy-mode-vi y send -X copy-selection-and-cancel
bind -T copy-mode-vi Escape send -X cancel

# ── Mouse support ─────────────────────────────────────────────────────────────
set -g mouse on

# ── Misc quality-of-life ──────────────────────────────────────────────────────
set -g base-index 1 # start window numbering at 1
setw -g pane-base-index 1 # start pane numbering at 1
set -g renumber-windows on # renumber when a window is closed
set -sg escape-time 0 # no delay for Escape key
set -g history-limit 10000 # larger scrollback buffer

# ── Copy mode ─────────────────────────────────────────────────────────────────
setw -g mode-keys vi
# Enter copy mode with Prefix + Space
bind Space copy-mode
# Vim-style selection and copy
bind -T copy-mode-vi v send -X begin-selection
bind -T copy-mode-vi V send -X select-line
bind -T copy-mode-vi C-v send -X rectangle-toggle
bind -T copy-mode-vi y send -X copy-pipe-and-cancel "pbcopy" # macOS
# bind -T copy-mode-vi y send -X copy-pipe-and-cancel "xclip -selection clipboard" # Linux X11
# bind -T copy-mode-vi y send -X copy-pipe-and-cancel "wl-copy" # Linux Wayland
bind -T copy-mode-vi Escape send -X cancel
```
