# Round Robin Scheduler - Makefile
# Simple commands to build and run TUI or Visual binaries

.PHONY: tui visual help

# Default target - show help
help:
	@echo "Round Robin Scheduler"
	@echo ""
	@echo "Available commands:"
	@echo "  make tui     - Build and run Terminal UI"
	@echo "  make visual  - Build and run Visual chart generator"
	@echo ""

# Build and run TUI (Terminal User Interface)
tui:
	@mkdir -p output
	@cargo run --bin cp-tui

# Build and run Visual chart generator
visual:
	@mkdir -p output
	@cargo run --bin cp-visual
