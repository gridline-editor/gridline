const std = @import("std");

const c_flags = [_][]const u8{
    "-std=c11",
    "-O0",
    "-g",
    "-Wall",
    "-Wextra",
    "-pedantic",
    "-Wvla",
    "-Wc++-compat",
};

const c_source_independent = [_][]const u8{
    "src/main.c",
    "src/utils/memory.c",
    "src/utils/stack_allocator.c",
    "src/utils/toml.c",
};

const c_include_paths = [_][]const u8{
    "src",
};

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "gridline",
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });

    exe.addCSourceFiles(.{
        .files = &c_source_independent,
        .flags = &c_flags,
    });

    exe.addIncludePath(b.path("src"));
    b.installArtifact(exe);
}
