#!/bin/bash

# Usage: ./build.sh [command] [options]

# Exit immediately if any command returns a non-zero status.
set -e

# ANSI colour codes used to make terminal output easier to read.
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Default configuration values.
BUILD_DIR="build"
BUILD_TYPE="Release"
# Use all available CPU cores for parallel compilation; fall back to 4 if nproc is unavailable.
JOBS=$(nproc 2>/dev/null || echo 4)

# Prints a usage summary with all supported commands and options.
show_help() {
    echo -e "${CYAN}CMake Build Script${NC}"
    echo ""
    echo -e "${YELLOW}Usage:${NC}"
    echo "  ./build.sh [command] [options]"
    echo ""
    echo -e "${YELLOW}Commands:${NC}"
    echo "  build         Build the project (default)"
    echo "  rebuild       Clean and build"
    echo "  clean         Remove the build directory"
    echo "  help          Show this help message"
    echo ""
    echo -e "${YELLOW}Options:${NC}"
    echo "  --verbose     Enable verbose build output"
    echo "  --jobs N      Use N parallel jobs (default: $JOBS)"
    echo "  --debug       Build in Debug mode (default: Release)"
    echo ""
    echo -e "${YELLOW}Examples:${NC}"
    echo "  ./build.sh build"
    echo "  ./build.sh rebuild --verbose"
    echo "  ./build.sh build --jobs 8 --debug"
}

# Removes the build directory if it exists; reports if there is nothing to clean.
clean_build() {
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        echo -e "${GREEN}Clean complete${NC}"
    else
        echo -e "${BLUE}No build directory found, nothing to clean${NC}"
    fi
}

# Creates the build directory, runs CMake configure, then compiles with make.
build_project() {
    echo -e "${YELLOW}Configuring CMake (Build Type: $BUILD_TYPE)...${NC}"
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"

    # Pass VERBOSE_MAKEFILE when verbose mode is requested so every compiler command is printed.
    if [ "$VERBOSE" = true ]; then
        cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_VERBOSE_MAKEFILE=ON ..
    else
        cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..
    fi

    cd ..

    # Symlink compile_commands.json to the project root so clangd picks it up automatically.
    ln -sf "$BUILD_DIR/compile_commands.json" compile_commands.json

    echo -e "${YELLOW}Building project...${NC}"
    cd "$BUILD_DIR"
    # Use VERBOSE=1 with make for extra per-file output in verbose mode.
    if [ "$VERBOSE" = true ]; then
        make -j"$JOBS" VERBOSE=1
    else
        make -j"$JOBS"
    fi
    cd ..

    echo -e "${GREEN}Build complete!${NC}"
    echo -e "${BLUE}Executable location: $BUILD_DIR/main${NC}"
}

# Parse all command-line arguments before executing any action.
VERBOSE=false
COMMAND=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --verbose)
            VERBOSE=true
            shift
            ;;
        --jobs)
            # Override the parallel job count with the next argument.
            JOBS="$2"
            shift 2
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --help)
            show_help
            exit 0
            ;;
        build|rebuild|clean|help)
            COMMAND="$1"
            shift
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            show_help
            exit 1
            ;;
    esac
done

# Default to the build command if the user provided no explicit command.
if [ -z "$COMMAND" ]; then
    COMMAND="build"
fi

echo -e "${BLUE}Starting $COMMAND...${NC}"

# Dispatch to the appropriate function based on the parsed command.
case $COMMAND in
    clean)
        clean_build
        ;;
    build)
        build_project
        ;;
    rebuild)
        # Rebuild performs a clean followed by a fresh build.
        clean_build
        build_project
        ;;
    help)
        show_help
        ;;
    *)
        echo -e "${RED}Unknown command: $COMMAND${NC}"
        show_help
        exit 1
        ;;
esac

echo -e "${GREEN}Done!${NC}"
