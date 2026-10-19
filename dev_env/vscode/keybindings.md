install vim extension

keybindings json
```
// Place your key bindings in this file to override the defaults
[
    // Navigation
    {
        "key": "ctrl+h",
        "command": "workbench.action.navigateLeft"
    },
    {
        "key": "ctrl+l",
        "command": "workbench.action.navigateRight"
    },
    {
        "key": "ctrl+k",
        "command": "workbench.action.navigateUp"
    },
    {
        "key": "ctrl+j",
        "command": "workbench.action.navigateDown"
    },
    {
        "key": "ctrl+alt+h",
        "command": "workbench.action.previousEditorInGroup",
        "when": "(vim.mode == 'Normal' || vim.mode == 'Visual') && (editorTextFocus || !inputFocus)",
    },
    {
        "key": "ctrl+alt+l",
        "command": "workbench.action.nextEditorInGroup",
        "when": "(vim.mode == 'Normal' || vim.mode == 'Visual') && (editorTextFocus || !inputFocus)",
    },
    {
        "key": "space e",
        "command": "workbench.action.showAllEditors",
        "when": "vim.mode == 'Normal' && (editorTextFocus || (!inputFocus && !findInputFocussed))",
    },
    {
        "key": "space s g",
        "command": "workbench.action.findInFiles",
        "when": "vim.mode == 'Normal' && (editorTextFocus || (!inputFocus && !findInputFocussed))",
    },
    {
        "key": "space space",
        "command": "workbench.action.quickOpen",
        "when": "vim.mode == 'Normal' && (editorTextFocus || (!inputFocus && !findInputFocussed))",
    },
    {
        "key": "space o s",
        "command": "workbench.action.toggleSidebarVisibility",
        "when": "vim.mode == 'Normal' && (editorTextFocus || (!inputFocus && !findInputFocussed))",
    },
    {
        "key": "space o e",
        "command": "workbench.view.explorer",
        "when": "vim.mode == 'Normal' && (editorTextFocus || (!inputFocus && !findInputFocussed))",
    },
    {
        "key": "space o o",
        "command": "outline.toggleVisibility",
        "when": "vim.mode == 'Normal' && (editorTextFocus || (!inputFocus && !findInputFocussed))",
    },
    {
        "key": "space o t",
        "command": "workbench.action.togglePanel",
        "when": "vim.mode == 'Normal' && (editorTextFocus || (!inputFocus && !findInputFocussed))",
    },
    {
        "key": "space f f",
        "command": "workbench.action.focusActiveEditorGroup",
        "when": "vim.mode == 'Normal' && (editorTextFocus || (!inputFocus && !findInputFocussed))",
    },
    {
        "key": "space f o",
        "command": "outline.focus",
        "when": "vim.mode == 'Normal' && (editorTextFocus || (!inputFocus && !findInputFocussed))",
    },
    {
        "key": "space f t",
        "command": "terminal.focus",
        "when": "vim.mode == 'Normal' && (editorTextFocus || (!inputFocus && !findInputFocussed))",
    },
    // Coding
    {
        "key": "escape",
        "command": "extension.vim_escape",
        "when": "vim.mode != 'Normal' && editorTextFocus",
    },
    {
        "key": "shift+j",
        "command": "editor.action.moveLinesDownAction",
        "when": "vim.mode != 'Insert' && editorTextFocus",
    },
    {
        "key": "shift+k",
        "command": "editor.action.moveLinesUpAction",
        "when": "vim.mode != 'Insert' && editorTextFocus",
    },
    {
        "key": "shift+d",
        "command": "editor.action.showHover",
        "when": "vim.mode == 'Normal' && editorTextFocus",
    },
    {
        "key": "space c a",
        "command": "editor.action.codeAction",
        "when": "vim.mode == 'Normal' && editorTextFocus",
    },
    {
        "key": "space c r",
        "command": "editor.action.rename",
        "when": "vim.mode == 'Normal' && editorTextFocus",
    },
    {
        "key": "space c s",
        "command": "workbench.action.gotoSymbol",
        "when": "vim.mode == 'Normal' && editorTextFocus",
    },
    {
        "key": "space c w",
        "command": "workbench.action.files.saveWithoutFormatting",
        "when": "vim.mode == 'Normal' && editorTextFocus",
    },
    {
        "key": "space l",
        "command": "workbench.action.gotoLine",
        "when": "vim.mode == 'Normal' && editorTextFocus",
    },
    {
        "key": "u",
        "command": "undo",
        "when": "vim.mode == 'Normal' && editorTextFocus",
    },
    {
        "key": "shift+u",
        "command": "redo",
        "when": "vim.mode == 'Normal' && editorTextFocus",
    },
    {
        "key": "space g d",
        "command": "editor.action.revealDefinition",
        "when": "vim.mode == 'Normal' && editorTextFocus",
    },
    {
        "key": "space g r",
        "command": "editor.action.goToReferences",
        "when": "vim.mode == 'Normal' && editorTextFocus",
    },
    {
        "key": "ctrl+f",
        "command": "actions.find",
        "when": "(vim.mode == 'Normal' || vim.mode == 'Visual') && (editorTextFocus || !inputFocus)"
    },
    {
        "key": "ctrl+d",
        "command": "editor.action.addSelectionToNextFindMatch",
        "when": "(vim.mode == 'Normal' || vim.mode == 'Visual') && (editorTextFocus || !inputFocus)"
    },
    {
        "key": "ctrl+w",
        "command": "workbench.action.closeActiveEditor",
        "when": "(vim.mode == 'Normal' || vim.mode == 'Visual') && (editorTextFocus || !inputFocus)"
    },
    // Explorer
    {
        "key": "r",
        "command": "renameFile",
        "when": "filesExplorerFocus && foldersViewVisible && !explorerResourceIsRoot && !explorerResourceReadonly && !inputFocus"
    },
    {
        "key": "c",
        "command": "filesExplorer.copy",
        "when": "filesExplorerFocus && foldersViewVisible && !explorerResourceIsRoot && !explorerResourceReadonly && !inputFocus"
    },
    {
        "key": "p",
        "command": "filesExplorer.paste",
        "when": "filesExplorerFocus && foldersViewVisible && !explorerResourceIsRoot && !explorerResourceReadonly && !inputFocus"
    },
    {
        "key": "x",
        "command": "filesExplorer.cut",
        "when": "filesExplorerFocus && foldersViewVisible && !explorerResourceIsRoot && !explorerResourceReadonly && !inputFocus"
    },
    {
        "key": "d",
        "command": "deleteFile",
        "when": "filesExplorerFocus && foldersViewVisible && !explorerResourceIsRoot && !explorerResourceReadonly && !inputFocus"
    },
    {
        "key": "a",
        "command": "explorer.newFile",
        "when": "filesExplorerFocus && foldersViewVisible && !explorerResourceIsRoot && !explorerResourceReadonly && !inputFocus"
    },
    {
        "key": "shift+a",
        "command": "explorer.newFolder",
        "when": "filesExplorerFocus && foldersViewVisible && !explorerResourceIsRoot && !explorerResourceReadonly && !inputFocus"
    },
    // Debug
    {
        "key": "shift+j",
        "command": "workbench.action.debug.stepOver",
        "when": "inDebugMode"
    },
    {
        "key": "shift+l",
        "command": "workbench.action.debug.stepInto",
        "when": "inDebugMode"
    },
    {
        "key": "shift+k",
        "command": "workbench.action.debug.continue",
        "when": "inDebugMode"
    },
    {
        "key": "shift+h",
        "command": "workbench.action.debug.stepOut",
        "when": "inDebugMode"
    },
    {
        "key": "shift+c",
        "command": "workbench.action.debug.stop",
        "when": "inDebugMode"
    },
    {
        "key": "ctrl+alt+b",
        "command": "-workbench.action.toggleAuxiliaryBar"
    },
]
```