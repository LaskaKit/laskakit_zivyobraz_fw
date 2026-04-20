##### HTML #####
html_template = """
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>LaskaKit zivyobraz FW</title>
</head>
<body>
  <h1>LaskaKit zivyobraz FW</h1>

  <script
    type="module"
    src="https://unpkg.com/esp-web-tools/dist/web/install-button.js?module">
  </script>

  <style>
  esp-web-install-button button[slot="activate"] {
      border-radius: 4px;
      padding: 2px 8px;
  }
  </style>

  <table border="1" cellpadding="8", cellspacing="0">
      <thead>
          <tr>
              <th>Size</th>
              <th>Colors</th>
              <th>Name</th>
              <th>ESPink v3.5+</th>
              <th>ESPink v2.7</th>
              <th>uESPink v1.2</th>
              <th>EPDIY v7</th>
          </tr>
      </thead>
      <tbody id="display-rows">
          {% for d_name, d_props in displays.items() %}
          <tr>
              <td>{{ d_props.size }}</td>
              <td><color-swatch scheme="{{ d_props.colorspace }}"></color-swatch></td>
              <td>{{ d_name }}</td>
              <td>
                  {% if 'espink-v3' in d_props %}
                  <esp-web-install-button data-fw-name="{{ d_props.get('espink-v3', '') }}">
                      <button slot="activate">Install</button>
                  </esp-web-install-button>
                  {% endif %}
              </td>
              <td>
                  {% if 'espink-v2' in d_props %}
                  <esp-web-install-button data-fw-name="{{ d_props.get('espink-v2', '') }}">
                      <button slot="activate">Install</button>
                  </esp-web-install-button>
                  {% endif %}
              </td>
              <td>
                  {% if 'uespink-v1' in d_props %}
                  <esp-web-install-button data-fw-name="{{ d_props.get('uespink-v1', '') }}">
                      <button slot="activate">Install</button>
                  </esp-web-install-button>
                  {% endif %}
              <td>
                {% if 'epdiy-v7' in d_props %}
                <esp-web-install-button data-fw-name="{{ d_props.get('epdiy-v7', '') }}">
                    <button slot="activate">Install</button>
                </esp-web-install-button>
                {% endif %}
              </td>
          </tr>
          {% endfor %}
      </tbody>
  </table>

  <!---->
  <!--Color Swatch web component-->
  <script>
      const SCHEMES = {
        BW:   ["black", "white"],
        RBW:  ["red", "black", "white"],
        YBW:  ["yellow", "black", "white"],
        BWRY: ["black", "white", "red", "yellow"],
        G4:   ["black", "#555", "#aaa", "white"],
        C4:   ["yellow", "red", "black", "white"],
        G8:   ["black", "#242424", "#484848", "#6d6d6d", "#919191", "#b6b6b6", "#dadada", "white"],
      };

      class ColorSwatch extends HTMLElement {
        connectedCallback() {
          const colors = SCHEMES[this.getAttribute("scheme")] ?? [];
          this.innerHTML = colors
            .map(c => {
              const border = c === "white" ? "box-shadow:inset 0 0 0 1px #ccc;" : "";
              return `<span style="background-color:${c};${border}" title="${c}"></span>`;
            })
            .join(" ");
        }
      }
      customElements.define("color-swatch", ColorSwatch);
    </script>
    <style>
        color-swatch span {
          display: inline-block;
          width: 0.9em;
          height: 0.9em;
          border-radius: 2px;
        }
    </style>
    <!---->

    <!-- Manifest generation -->
    <script>
      const board_chip_family = {
          "espink-v2": "ESP32",
          "espink-v2-gxepd": "ESP32",
          "espink-v2-bbep": "ESP32",
          "espink-v3": "ESP32-S3",
          "espink-v3-gxepd": "ESP32-S3",
          "espink-v3-bbep": "ESP32-S3",
          "uespink-v1": "ESP32-S3",
          "uespink-v1-gxepd": "ESP32-S3",
          "uespink-v1-bbep": "ESP32-S3",
          "epdiy-v7-fastepd": "ESP32-S3",
      }

      const board_display_name = {
          "espink-v2": "ESPink_V2.6(7)",
          "espink-v2-gxepd": "ESPink_V2.6(7)",
          "espink-v2-bbep": "ESPink_V2.6(7)",
          "espink-v3": "ESPink_V3.5+",
          "espink-v3-gxepd": "ESPink_V3.5+",
          "espink-v3-bbep": "ESPink_V3.5+",
          "uespink-v1": "Micro ESPink_V1.2+",
          "uespink-v1-gxepd": "Micro ESPink_V1.2+",
          "uespink-v1-bbep": "Micro ESPink_V1.2+",
          "epdiy-v7-fastepd": "EPDiy v7",
      }

      function makeManifest(fwName) {
        const [, board, display] = fwName.split("_")
        // const BASE_URL = `https://github.com/LaskaKit/laskakit_zivyobraz_fw/releases/v${version.replaceAll("-", ".")}`
        const BASE_URL = `http://localhost:8080`
        const manifest = {
          name:  `Živý obraz ${board_display_name[board]} ${display}`,
          version: "dev",
          builds: [
            {
              chipFamily: `${board_chip_family[board]}`,
              parts: [{ path: `${BASE_URL}/artifacts/${fwName}`, offset: 0 }]
            }
          ]
        };
        const blob = new Blob([JSON.stringify(manifest)], { type: "application/json" });
        return URL.createObjectURL(blob);
      }

      document.querySelectorAll("esp-web-install-button[data-fw-name]").forEach(btn => {
        btn.manifest = makeManifest(btn.dataset.fwName);
      });
    </script>
    <!---->

</body>
</html>

"""


###### SCRIPT ######

from pathlib import Path
from jinja2 import Environment

def get_display_info(line):
    line = line.split()
    return line[2], {
            "size": line[0],
            "colorspace": line[1],
        }

def get_boards(firmwares):
    boards = [fw.split("_")[1] for fw in firmwares]
    boards = [board.rstrip("-gxepd").rstrip("-bb").rstrip("-fast") for board in boards]
    boards = list(set(boards))
    return boards

env = Environment()
template = env.from_string(html_template)

with open("displays.txt") as f:
    rows = f.readlines()
    header = rows[0]
    data = rows[1:]

    displays = {name: props for name, props in [get_display_info(line) for line in data]}
    firmwares = [p.name for p in Path("./artifacts").glob("*.bin")]

    boards = get_boards(firmwares)
    for d_name, d_props in displays.items():
        for board in boards:
            for fw in firmwares:
                if board in fw and d_name in fw:
                    d_props[board] = fw

    print(template.render(firmwares=firmwares, displays=displays))
