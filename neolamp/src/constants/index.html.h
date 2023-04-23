const char index_html[] PROGMEM = R"rawliteral("
<!DOCTYPE html>
<html>
  <head>
    <title>Nachtlicht</title>
    <meta name="viewport" content="width=device-width, initial-scale=1" />

    <style>
      html {
        font-family: Times New Roman;
        display: inline-block;
        text-align: center;
      }
      h2 {
        font-size: 2rem;
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
    <h2>Nachtlicht</h2>
    <h2>Einstellungen</h2>
    <form action="/get" target="hidden-form" id="getForm">
      <table style="margin-left: auto; margin-right: auto; width: 20em">
        <tr>
          <td style="text-align: right; padding-right: 1em">Helligkeit:</td>
          <td>
            <input
              name="input_brightness"
              value="%input_brightness%"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              style="width: 15em"
            />
          </td>
        </tr>
        <tr>
          <td><hr /></td>
          <td><hr /></td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">Aufstehzeit:</td>
          <td>
            <input
              type="text"
              name="input_wakeup_time"
              value="%input_wakeup_time%"
              style="width: 15em"
              placeholder="8:00"
            />
          </td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">Animationszeit:</td>
          <td>
            <input
              type="text"
              name="input_animation_time"
              value="%input_animation_time%"
              style="width: 15em"
              placeholder="08:30"
            />
          </td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">Schlafenszeit:</td>
          <td>
            <input
              type="text"
              name="input_sleep_time"
              value="%input_sleep_time%"
              style="width: 15em"
              placeholder="19:00"
            />
          </td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">
            Animation am Tag:
          </td>
          <td>
            <select
              name="input_animation"
              style="width: 15em"
              onchange="handle_select_on_change()"
            >
              %input_animation%
            </select>
          </td>
        </tr>
        <tr>
          <td><hr /></td>
          <td><hr /></td>
        </tr>
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