const char timer_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>%input_name%</title>
    <meta name="viewport" content="width=device-width, initial-scale=1" />

    <style>
      html {
        display: inline-block;
        text-align: center;
      }
      h2 {
        font-size: 1.5rem;
        color: #ffaa00;
      }
      table {
        margin-left: auto;
        margin-right: auto;
      }
      td.left {
        text-align: right;
      }
      td.middle {
        text-align: left;
        width: 128px;
        padding-left: 0.5em;
      }
      td.right {
        text-align: left;
      }
      input.table_input {
        width: 128px;
      }
      select.table_input {
        width: 128px;
      }
      button.bigfont {
        font-size: 1.6rem;
      }
      button,
      a {
        font-family: "Cooper Black", serif;
        background-color: #3522dd; /* Green */
        border: solid;
        border-color: white;
        color: white;
        /* padding: 15px 32px; */
        text-align: center;
        text-decoration: none;
        display: inline-block;
        margin-right: auto;
        width: 256px;
        height: 64px;
        font-size: 1.2rem;
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
    <table>
      <tr>
        <td class="left">Timer:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getTimer">
            <input
              type="time"
              name="iTimer"
              value="%iTimer%"
              class="table_input"
              step="1"
              onchange="handle_onchange_and_reload('getTimer')"
            />
          </form>
        </td>
        <td class="right"></td>
      </tr>
    </table>
    <br />
    <form action="/get" target="hidden-form" id="sT">
      <button
        class="bigfont"
        name="sTimer"
        value="sTimer"
        orient="vertical"
        onclick="handle_onchange_and_reload('sT')"
      >
        %sTimer%
      </button>
    </form>
    <br />
    <br />
    <a href="/"><br />Zur Startseite</a>
    <br />
    <br />
    <br />
    <br />
    <iframe style="display: none" name="hidden-form"></iframe>
  </body>
</html>
)rawliteral";
