Import("env")


import os
from util.manifest import ZivyObrazManifest

display_type = os.environ.get("DISPLAY_TYPE", "DISPLAY_NONE")
board = env.get("PIOENV")
version = env.GetProjectConfig().get("common", "version")

APP_BIN = "$BUILD_DIR/${PROGNAME}.bin"
MERGED_BIN = f"artifacts/zo_{board}_{display_type.split("_")[1]}_{version.replace(".", "-")}.bin"
# MERGED_BIN = "$BUILD_DIR/${PROGNAME}_merged.bin"
BOARD_CONFIG = env.BoardConfig()

def merge_bin(source, target, env):
    # create artifacts directory if it does not exist
    os.makedirs("artifacts", exist_ok=True)

    # The list contains all extra images (bootloader, partitions, eboot) and
    # the final application binary
    flash_images = env.Flatten(env.get("FLASH_EXTRA_IMAGES", [])) + ["$ESP32_APP_OFFSET", str(target[0])]

    # Run esptool to merge images into a single binary
    env.Execute(
        " ".join(
            [
                "$PYTHONEXE",
                "$OBJCOPY",
                "--chip",
                BOARD_CONFIG.get("build.mcu", "esp32"),
                "merge_bin",
                "-o",
                MERGED_BIN,
            ]
            + flash_images
        )
    )

def create_manifest(source, target, env):
    manifest = ZivyObrazManifest.from_bin_filename(MERGED_BIN)
    with open(f"{manifest.output_dir}/{manifest.json_filename()}", "w") as f:
        f.write(manifest.json_manifest())

# Add a post action that runs esptoolpy to merge available flash images
env.AddPostAction(APP_BIN, merge_bin)
env.AddPostAction(APP_BIN, create_manifest)

# Patch the upload command to flash the merged binary at address 0x0
env.Replace(
    UPLOADERFLAGS=[
            f
            for f in env.get("UPLOADERFLAGS")
            if f not in env.Flatten(env.get("FLASH_EXTRA_IMAGES"))
        ]
        + ["0x0", MERGED_BIN],
    UPLOADCMD='"$PYTHONEXE" "$UPLOADER" $UPLOADERFLAGS',
)
