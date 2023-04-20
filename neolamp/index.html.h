const char index_html[] PROGMEM = R"rawliteral("
<!DOCTYPE html>
<html>
  <head>
    <title>HTML Form to Input Data</title>
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
      function handle_new_input() {
        setTimeout(function () {
          document.location.reload(false);
        }, 500);
      }
    </script>
  </head>
  <body>
    <h2>Lampe - Einstellungen</h2>
    <form action="/get" target="hidden-form">
      <table style="margin-left: auto; margin-right: auto">
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
          <td style="text-align: right; padding-right: 1em">Zeitzone:</td>
          <td>
            <select name="timezone" style="width: 15em">
              <option value="berlin">Berlin</option>
              <option value="usa">USA</option>
              <option value="bla">bla</option>
              <option value="blubb">blubb</option>
            </select>
          </td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">
            Animation am Tag:
          </td>
          <td>
            <select name="animation" style="width: 15em">
              <option value="mix">Farbkeise und Pulsieren</option>
              <option value="circle">Farbkeise</option>
              <option value="pulse">Pulsieren</option>
              <option value="green">Gruen</option>
              <option value="off">Lampe Ausgeschaltet</option>
            </select>
          </td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">Helligkeit:</td>
          <td>
            <input
              name="brightness"
              id="brightness"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              style="width: 15em"
            />
          </td>
        </tr>
        <tr>
          <td style="text-align: right; padding-right: 1em">
            <br />Uhrzeit:<br />
            <label style="font-size: 0.75em"> (beim Laden der Seite)</label>
          </td>
          <td style="width: 15em">
            <br />
            XX:XX
            <br />
            <label style="font-size: 0.75em"> (wird nicht aktualisiert)</label>
          </td>
        </tr>
      </table>
      <br />
      <br />
      <input
        type="submit"
        value="Einstellungen speichern"
        onclick="handle_new_input()"
        style="width: 30em"
      />
    </form>
    <br />
    <br />
    <br />
    <iframe style="display: none" name="hidden-form"></iframe>
  </body>
</html>
)rawliteral";
