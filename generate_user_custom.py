Import("env")
import os

firmware_name = env.GetProjectOption("custom_firmware_name")
firmware_version = env.GetProjectOption("custom_firmware_version")
firmware_suffix = env.GetProjectOption("custom_firmware_suffix")
firmware_dir = env.GetProjectOption("custom_firmware_dir")
if not os.path.exists(firmware_dir):
    os.makedirs(firmware_dir)

firmware_bin = "%s_%s%s" % (firmware_name, firmware_version, firmware_suffix)
firmware_path = os.path.join(firmware_dir, firmware_bin)

app_bin = os.path.join("$BUILD_DIR", "${PROGNAME}%s" % firmware_suffix)

MERGE_COMMAND = '"$PYTHONEXE" "$UPLOADER" --chip esp32 merge_bin -o %s %s $ESP32_APP_OFFSET %s' % (
    firmware_path,
    " ".join([addr + " " + name for addr, name in env["FLASH_EXTRA_IMAGES"]]),
    app_bin,
)

env.AddCustomTarget(
    name="firmware",
    dependencies=["buildfs"],
    actions=[MERGE_COMMAND],
    title="Generate User Custom",
)

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", env.VerboseAction(MERGE_COMMAND, "Building %s" % firmware_path))
