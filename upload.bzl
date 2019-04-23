def __create_upload_script():
    native.genrule(
        name = "internal_upload_script",
        outs = ["upload.sh"],
        cmd = "echo 'sudo dfu-programmer atmega32u4 erase; sudo dfu-programmer atmega32u4 flash $$1; sudo dfu-programmer atmega32u4 reset' > $@",
    )

def upload(name, srcs = []):
    if not "internal_upload_script" in native.existing_rules():
        __create_upload_script()
    native.sh_binary(
        name = name,
        srcs = ["upload.sh"],
        args = ["$(location {input})".format(input = srcs[0])],
        data = [srcs[0]],
    )
