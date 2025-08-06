const std = @import("std");

const c_sources = [_][]const u8{
    "src/main.c",
    "src/utils/memory.c",
    "src/utils/stack_allocator.c",
    "src/utils/toml.c",
};

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "gridline",
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });

    exe.addIncludePath(b.path("src"));

    const allocator = b.allocator;
    var db_pathnames = std.ArrayList([]const u8).init(allocator);
    defer db_pathnames.deinit();

    for (c_sources) |filename| {
        const basename = std.fs.path.basename(filename);
        const db_path = try std.fs.path.join(allocator, &.{
            b.cache_root.path.?,
            try std.fmt.allocPrint(allocator, "{s}.json", .{basename}),
        });
        try db_pathnames.append(db_path);

        exe.addCSourceFile(.{
            .file = b.path(filename),
            .flags = &.{
                "-std=c11",
                "-O0",
                "-g",
                "-Wall",
                "-Wextra",
                "-pedantic",
                "-Wvla",
                "-Wc++-compat",
                "-MJ",
                db_path,
            },
        });
    }

    b.installArtifact(exe);

    // TODO: join json files manually, avoid system dependancy
    const sed_cmd_str = try std.fmt.allocPrint(
        allocator,
        "sed -e '1s/^/[/' -e '$s/,$/]/' {s}/*.json > compile_commands.json",
        .{b.cache_root.path.?},
    );

    const sed_step = b.addSystemCommand(&.{ "sh", "-c", sed_cmd_str });
    const merge_step = b.step("compile-db", "Generate compile_commands.json");
    merge_step.dependOn(&sed_step.step);
}
