import json
import os
from dataclasses import dataclass


manifest_template = {
    "name": "Živý obraz",
    "version": "@",
    "home_assistant_domain": "zivy_obraz",
    "builds": [
        {
            "chipFamily": "@",
            "parts": [
                {
                    "path": "@",
                    "offset": 0
                }
            ]
        }
    ]
}


board_chipFamily = {
    "espink-v2": "ESP32",
    "espink-v3": "ESP32-S3",
}

board_displayName = {
    "espink-v2": "ESPink_V2.6(7)",
    "espink-v3": "ESPink_V3.5+",
}


@dataclass
class ZivyObrazManifest:
    prefix: str
    board: str
    display: str
    version: str
    bin_filename: str
    output_dir: str

    @classmethod
    def from_bin_filename(cls, bin_filename):
        output_dir = os.path.dirname(bin_filename)
        if output_dir:
            output_dir += "/"
        bin_filename = os.path.basename(bin_filename)
        name, ext = bin_filename.split('.')
        prefix, board, display, version = name.split("_")
        return cls(prefix, board, display, version, bin_filename, output_dir)

    def json_filename(self):
        return f"{self.prefix}_{self.board}_{self.display}.json"

    def json_manifest(self):
        manifest = manifest_template.copy()
        manifest["name"] = f"Živý obraz - {board_displayName[self.board]} {self.display}"
        manifest["version"] = self.version.replace('-', '.')
        manifest["builds"][0]["chipFamily"] = board_chipFamily[self.board]
        manifest["builds"][0]["parts"][0] = self.bin_filename
        return json.dumps(manifest, indent=2, ensure_ascii=False)
