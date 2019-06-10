def _doxygen_archive_impl(ctx):
    """Generate a .tar.gz archive containing documentation using Doxygen.

    Args:
        name: label for the generated rule. The archive will be "%{name}.tar.gz".
        doxyfile: configuration file for Doxygen, @@OUTPUT_DIRECTORY@@ will be replaced with the actual output dir
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
    """
    Generates a sphinx documentation archive (.tar.gz).
    The output is called <name>.tar.gz, where <name> is the
    name of the rule.

    Args:
        config_file: sphinx conf.py file

        doxygen_xml_archive: an archive that containing the generated doxygen
            xml files to be consumed by the breathe sphinx plugin.
            Setting this attribute automatically enables the breathe plugin

        srcs: the *.rst files to consume
    """
    out_file = ctx.outputs.sphinx
    out_dir_path = out_file.short_path[:-len(".tar.gz")]
    commands = ["mkdir _static"]
    if ctx.attr.doxygen_xml_archive != None:
        commands = commands + [
            "mkdir xml",
            "tar -xzf {xml} -C xml --strip-components=2".format(xml = ctx.file.doxygen_xml_archive.path),
        ]

    commands = commands + [
        "sphinx-build -M build ./ _build -q -b html -C {settings}".format(
            settings = _sphinx_settings(ctx),
            out_dir = out_dir_path,
        ),
    ]
    commands = commands + [
        "tar czf %s -C _build/build/ ./" % (out_file.path),
    ]
    ctx.actions.run_shell(
        use_default_shell_env = True,
        outputs = [out_file],
        inputs = ctx.files.srcs + [ctx.file.doxygen_xml_archive],
        command = " && ".join(commands),
    )

sphinx_archive = rule(
    implementation = _sphinx_archive_impl,
    attrs = {
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = True,
        ),
        "doxygen_xml_archive": attr.label(
            default = None,
            allow_files = True,
            single_file = True,
        ),
        "master_doc": attr.string(default = "contents"),
        "version": attr.string(
            mandatory = True,
        ),
        "project": attr.string(
            default = "",
        ),
        "copyright": attr.string(default = ""),
        "extensions": attr.string_list(default = [
            "sphinx.ext.intersphinx",
            "sphinx.ext.todo",
        ]),
        "templates": attr.string_list(default = []),
        "source_suffix": attr.string_list(default = [".rst"]),
        "exclude_patterns": attr.string_list(default = ["_build", "Thumbs.db", ".DS_Store"]),
        "pygments_style": attr.string(default = ""),
        "language": attr.string(default = ""),
        "html_theme": attr.string(default = "sphinx_rtd_theme"),
        "html_theme_options": attr.string_dict(default = {}),
        "html_static_path": attr.string_list(default = ["_static"]),
        "html_sidebars": attr.string_dict(default = {}),
        "intersphinx_mapping": attr.string_dict(default = {}),
    },
    outputs = {
        "sphinx": "%{name}.tar.gz",
    },
)

def add_option(settings, setting, value):
    if value != None or len(value) == 0:
        settings = settings + ["-D {setting}={value}".format(setting = setting, value = value.replace(" ", "\ "))]
    return settings

def _sphinx_settings(ctx):
    settings = []
    extensions = ctx.attr.extensions
    settings = add_option(settings, "version", ctx.attr.version)
    if ctx.attr.project == "":
        settings = add_option(settings, "project", ctx.workspace_name)
    else:
        settings = add_option(settings, "project", ctx.attr.project)
    if ctx.attr.doxygen_xml_archive != None:
        extensions = extensions + ["breathe"]
        settings = add_option(settings, "breathe_projects." + ctx.workspace_name, "xml")
        settings = add_option(settings, "breathe_default_project", ctx.workspace_name)
    settings = add_option(settings, "copyright", ctx.attr.copyright)
    settings = add_option(settings, "master_doc", ctx.attr.master_doc)
    for extension in extensions:
        settings = add_option(settings, "extensions", extension)
    for template in ctx.attr.templates:
        settings = add_option(settings, "templates", template)
    for suffix in ctx.attr.source_suffix:
        settings = add_option(settings, "source_suffix", suffix)
    for pattern in ctx.attr.exclude_patterns:
        settings = add_option(settings, "exclude_patterns", pattern)
    settings = add_option(settings, "html_theme", ctx.attr.html_theme)
    for path in ctx.attr.html_static_path:
        settings = add_option(settings, "html_static_path", path)
    setting_string = " ".join(settings)
    return setting_string
