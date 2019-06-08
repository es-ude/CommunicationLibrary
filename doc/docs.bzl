def _doxygen_archive_impl(ctx):
    """Generate a .tar.gz archive containing documentation using Doxygen.

    Args:
        name: label for the generated rule. The archive will be "%{name}.tar.gz".
        doxyfile: configuration file for Doxygen
        srcs: source files the documentation will be generated from.
    """
    doxyfile = ctx.file.doxyfile
    out_file = ctx.outputs.out
    out_dir_path = out_file.short_path[:-len(".tar.gz")]
    commands = [
        "mkdir -p %s" % out_dir_path,
        "out_dir_path=$(cd %s; pwd)" % out_dir_path,
        "pushd %s" % doxyfile.dirname,
        """sed -e \"s:@@OUTPUT_DIRECTORY@@:$out_dir_path/:\" <%s | doxygen -""" % doxyfile.basename,
        "popd",
        "tar czf %s -C %s ./" % (out_file.path, out_dir_path),
    ]
    ctx.actions.run_shell(
        inputs = ctx.files.srcs + [doxyfile],
        outputs = [out_file],
        use_default_shell_env = True,
        command = " && ".join(commands),
    )

doxygen_archive = rule(
    implementation = _doxygen_archive_impl,
    attrs = {
        "doxyfile": attr.label(
            mandatory = True,
            allow_files = True,
            single_file = True,
        ),
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = True,
        ),
    },
    outputs = {
        "out": "%{name}.tar.gz",
    },
)

def _sphinx_archive_impl(ctx):
    out_file = ctx.outputs.sphinx
    out_dir_path = out_file.short_path[:-len(".tar.gz")]
    commands = ["mkdir _static"]
    commands = commands + [
        "mkdir xml",
        "tar -xzvf {xml} -C xml --strip-components=2".format(xml = ctx.file.doxygen_xml_archive.path),
    ]

    commands = commands + ["sphinx-build -M build ./ _build -b html -c {config_file}".format(config_file = ctx.file.config_file.dirname, out_dir = out_dir_path)]
    commands = commands + [
        "tar czf %s -C _build/build/ ./" % (out_file.path),
    ]
    ctx.actions.run_shell(
        use_default_shell_env = True,
        outputs = [out_file],
        inputs = ctx.files.srcs + [ctx.file.config_file, ctx.file.doxygen_xml_archive],
        command = ";\n".join(commands),
    )

sphinx_archive = rule(
    implementation = _sphinx_archive_impl,
    attrs = {
        "config_file": attr.label(
            mandatory = True,
            allow_files = True,
            single_file = True,
        ),
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = True,
        ),
        "doxygen_xml_archive": attr.label(
            default = None,
            allow_files = True,
            single_file = True,
        ),
    },
    outputs = {
        "sphinx": "%{name}-sphinx-html.tar.gz",
    },
)
