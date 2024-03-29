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
      function handle_onchange_and_reload(x) {
        document.getElementById(x).submit();
        setTimeout(function () {
          document.location.reload(false);
        }, 1000);
      }
    </script>
  </head>
  <body>
    <h2>%input_name%</h2>
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
          <form action="/get" target="hidden-form" id="getIWT">
            <input
              type="time"
              name="input_wakeup_time"
              value="%input_wakeup_time%"
              style="width: 15em"
              onchange="handle_onchange_and_reload('getIWT')"
            />
          </form>
        </td>
      </tr>
      <tr>
        <td style="text-align: right; padding-right: 1em">Lichtmodus:</td>
        <td>
          <form action="/get" target="hidden-form" id="getIWM">
            <select
              name="input_wakeup_mode"
              style="width: 15em"
              onchange="handle_onchange_and_reload('getIWM')"
            >
              %input_wakeup_mode%
            </select>
          </form>
        </td>
      </tr>
      <tr %wakeup_color_row%>
        <td style="text-align: right; padding-right: 1em">Farbe:</td>
        <td>
          <form action="/get" target="hidden-form" id="getIWC">
            <input
              type="color"
              name="input_wakeup_color"
              value="%input_wakeup_color%"
              style="width: 15em"
              onchange="handle_onchange_and_reload('getIWC')"
            />
          </form>
        </td>
      </tr>
      <tr>
        <td style="text-align: right; padding-right: 1em">Helligkeit:</td>
        <td>
          <form action="/get" target="hidden-form" id="getIWB">
            <input
              name="input_wakeup_brightness"
              value="%input_wakeup_brightness%"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              style="width: 15em"
              onchange="handle_onchange_and_reload('getIWB')"
            />
          </form>
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
          <form action="/get" target="hidden-form" id="getIDT">
            <input
              type="time"
              name="input_daytime_time"
              value="%input_daytime_time%"
              style="width: 15em"
              onchange="handle_onchange_and_reload('getIDT')"
            />
          </form>
        </td>
      </tr>
      <tr>
        <td style="text-align: right; padding-right: 1em">Lichtmodus:</td>
        <td>
          <form action="/get" target="hidden-form" id="getIDM">
            <select
              name="input_daytime_mode"
              style="width: 15em"
              onchange="handle_onchange_and_reload('getIDM')"
            >
              %input_daytime_mode%
            </select>
          </form>
        </td>
      </tr>
      <tr %daytime_color_row%>
        <td style="text-align: right; padding-right: 1em">Farbe:</td>
        <td>
          <form action="/get" target="hidden-form" id="getIDC">
            <input
              type="color"
              name="input_daytime_color"
              value="%input_daytime_color%"
              style="width: 15em"
              onchange="handle_onchange_and_reload('getIDC')"
            />
          </form>
        </td>
      </tr>
      <tr>
        <td style="text-align: right; padding-right: 1em">Helligkeit:</td>
        <td>
          <form action="/get" target="hidden-form" id="getIDB">
            <input
              name="input_daytime_brightness"
              value="%input_daytime_brightness%"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              style="width: 15em"
              onchange="handle_onchange_and_reload('getIDB')"
            />
          </form>
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
          <form action="/get" target="hidden-form" id="getIST">
            <input
              type="time"
              name="input_sleep_time"
              value="%input_sleep_time%"
              style="width: 15em"
              onchange="handle_onchange_and_reload('getIST')"
            />
          </form>
        </td>
      </tr>
      <tr>
        <td style="text-align: right; padding-right: 1em">Lichtmodus:</td>
        <td>
          <form action="/get" target="hidden-form" id="getISM">
            <select
              name="input_sleep_mode"
              style="width: 15em"
              onchange="handle_onchange_and_reload('getISM')"
            >
              %input_sleep_mode%
            </select>
          </form>
        </td>
      </tr>
      <tr %sleeptime_color_row%>
        <td style="text-align: right; padding-right: 1em">Farbe:</td>
        <td>
          <form action="/get" target="hidden-form" id="getISC">
            <input
              type="color"
              name="input_sleep_color"
              value="%input_sleep_color%"
              style="width: 15em"
              onchange="handle_onchange_and_reload('getISC')"
            />
          </form>
        </td>
      </tr>
      <tr>
        <td style="text-align: right; padding-right: 1em">Helligkeit:</td>
        <td>
          <form action="/get" target="hidden-form" id="getISB">
            <input
              name="input_sleep_brightness"
              value="%input_sleep_brightness%"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              style="width: 15em"
              onchange="handle_onchange_and_reload('getISB')"
            />
          </form>
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
          <form action="/get" target="hidden-form" id="getTZ">
            <select
              name="input_timezone"
              style="width: 15em"
              onchange="handle_onchange_and_reload('getTZ')"
            >
              %input_timezone%
            </select>
          </form>
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
    <br />
    <br />
    <br />
    <iframe style="display: none" name="hidden-form"></iframe>
  </body>
</html>
)rawliteral";
