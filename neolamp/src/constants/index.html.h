const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>%input_name%</title>
    <meta name="viewport" content="width=device-width, initial-scale=1" />

    <style>
      html {
        font-family: Times New Roman;
        display: inline-block;
        text-align: center;
      }
      h2 {
        font-size: 1.5rem;
        color: #ffaa00;
      }
    </style>

    <script>
      function handle_select_on_change() {
        document.getElementById("getForm").submit();
        setTimeout(function () {
          document.location.reload(false);
        }, 500);
      }
      function handle_new_input() {
        setTimeout(function () {
          document.location.reload(false);
        }, 500);
      }
    </script>
  </head>
  <body>
    <h2>%input_name%</h2>
    <form action="/get" target="hidden-form" id="getForm">
      <!----------------------------------------------------->
      <!-- WakeUp Time -->
      <table style="margin-left: auto; margin-right: auto; width: 20em">
        <tr>
          <td><hr /></td>
          <td><hr /></td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">Aufstehzeit:</td>
          <td>
            <input
              type="time"
              name="input_wakeup_time"
              value="%input_wakeup_time%"
              style="width: 15em"
              placeholder="8:00"
            />
          </td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">Lichtmodus:</td>
          <td>
            <select
              name="input_wakeup_mode"
              style="width: 15em"
              onchange="handle_select_on_change()"
            >
              %input_wakeup_mode%
            </select>
          </td>
        </tr>
        <tr %wakeup_color_row%>
          <td style="text-align: right; padding-right: 1em">Farbe:</td>
          <td>
            <input
              type="color"
              name="input_wakeup_color"
              value="%input_wakeup_color%"
              style="width: 15em"
              onchange="handle_select_on_change()"
            />
          </td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">Helligkeit:</td>
          <td>
            <input
              name="input_wakeup_brightness"
              value="%input_wakeup_brightness%"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              style="width: 15em"
              onchange="handle_select_on_change()"
            />
          </td>
        </tr>
        <tr>
          <td><hr /></td>
          <td><hr /></td>
        </tr>
        <!----------------------------------------------------->
        <!-- Daytime -->
        <tr>
          <td style="text-align: right; padding-right: 1em">Tags:</td>
          <td>
            <input
              type="time"
              name="input_daytime_time"
              value="%input_daytime_time%"
              style="width: 15em"
              placeholder="08:30"
            />
          </td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">Lichtmodus:</td>
          <td>
            <select
              name="input_daytime_mode"
              style="width: 15em"
              onchange="handle_select_on_change()"
            >
              %input_daytime_mode%
            </select>
          </td>
        </tr>
        <tr %daytime_color_row%>
          <td style="text-align: right; padding-right: 1em">Farbe:</td>
          <td>
            <input
              type="color"
              name="input_daytime_color"
              value="%input_daytime_color%"
              style="width: 15em"
              onchange="handle_select_on_change()"
            />
          </td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">Helligkeit:</td>
          <td>
            <input
              name="input_daytime_brightness"
              value="%input_daytime_brightness%"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              style="width: 15em"
              onchange="handle_select_on_change()"
            />
          </td>
        </tr>
        <tr>
          <td><hr /></td>
          <td><hr /></td>
        </tr>
        <!----------------------------------------------------->
        <!-- Sleep Time  -->

        <tr>
          <td style="text-align: right; padding-right: 1em">Schlafenszeit:</td>
          <td>
            <input
              type="time"
              name="input_sleep_time"
              value="%input_sleep_time%"
              style="width: 15em"
              placeholder="19:00"
            />
          </td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">Lichtmodus:</td>
          <td>
            <select
              name="input_sleep_mode"
              style="width: 15em"
              onchange="handle_select_on_change()"
            >
              %input_sleep_mode%
            </select>
          </td>
        </tr>
        <tr %sleeptime_color_row%>
          <td style="text-align: right; padding-right: 1em">Farbe:</td>
          <td>
            <input
              type="color"
              name="input_sleep_color"
              value="%input_sleep_color%"
              style="width: 15em"
              onchange="handle_select_on_change()"
            />
          </td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">Helligkeit:</td>
          <td>
            <input
              name="input_sleep_brightness"
              value="%input_sleep_brightness%"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              style="width: 15em"
              onchange="handle_select_on_change()"
            />
          </td>
        </tr>
        <tr>
          <td><hr /></td>
          <td><hr /></td>
        </tr>
        <!----------------------------------------------------->
        <!-- Time Zone -->
        <tr>
          <td style="text-align: right; padding-right: 1em">Zeitzone:</td>
          <td>
            <select
              name="input_timezone"
              style="width: 15em"
              onchange="handle_select_on_change()"
            >
              %input_timezone%
            </select>
          </td>
        </tr>
        <tr>
          <td><hr /></td>
          <td><hr /></td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">
            Uhrzeit:<br />
            <label style="font-size: 0.75em"> (der Lampe)</label>
          </td>
          <td style="width: 15em">
            %input_time_on_load%
            <br />
            <label style="font-size: 0.75em"> (wird nicht aktualisiert)</label>
          </td>
        </tr>
        <tr>
          <td><hr /></td>
          <td><hr /></td>
        </tr>
      </table>
      <br />
      <input
        type="submit"
        value="Einstellungen speichern"
        onclick="handle_new_input()"
        style="width: 20em; height: 2.5em"
      />
    </form>
    <br />
    <br />
    <br />
    <iframe style="display: none" name="hidden-form"></iframe>
  </body>
</html>
)rawliteral";
