const char settings_html[] PROGMEM = R"rawliteral(
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
        <td><hr /></td>
        <td><hr /></td>
        <td><hr /></td>
      </tr>
      <tr>
        <td class="left">Aufstehzeit:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIWT">
            <input
              type="time"
              name="iWT"
              value="%iWT%"
              class="table_input"
              onchange="handle_onchange_and_reload('getIWT')"
            />
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td class="left">Lichtmodus:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIWM">
            <select
              name="iWM"
              class="table_input"
              onchange="handle_onchange_and_reload('getIWM')"
            >
              %iWM%
            </select>
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr %hide_wakup_color%>
        <td class="left">Farbe:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIWC">
            <input
              type="color"
              name="iWC"
              value="%iWC%"
              class="table_input"
              onchange="handle_onchange_and_reload('getIWC')"
            />
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td class="left">Helligkeit:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIWB">
            <input
              name="iWB"
              value="%iWB%"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              class="table_input"
              onchange="handle_onchange_and_reload('getIWB')"
              oninput="valueIWB.innerHTML=iWB.value + ' &#0037;'"
            />
          </form>
        </td>
        <td class="right" id="valueIWB">%iWB% &#0037;</td>
      </tr>
      <tr %hide_blink%>
        <td class="left">Blink:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIBBlink">
            <select
              name="input_wakeup_blink"
              class="table_input"
              onchange="handle_onchange_and_reload('getIBBlink')"
            >
              %input_wakeup_blink%
            </select>
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td><hr /></td>
        <td><hr /></td>
        <td><hr /></td>
      </tr>
      <!----------------------------------------------------->
      <!-- Daytime -->
      <tr>
        <td class="left">Tags:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIDT">
            <input
              type="time"
              name="iDT"
              value="%iDT%"
              class="table_input"
              onchange="handle_onchange_and_reload('getIDT')"
            />
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td class="left">Lichtmodus:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIDM">
            <select
              name="iDM"
              class="table_input"
              onchange="handle_onchange_and_reload('getIDM')"
            >
              %iDM%
            </select>
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr %hide_daytime_color%>
        <td class="left">Farbe:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIDC">
            <input
              type="color"
              name="iDC"
              value="%iDC%"
              class="table_input"
              onchange="handle_onchange_and_reload('getIDC')"
            />
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr class="middle">
        <td class="left">Helligkeit:</td>
        <td>
          <form action="/get" target="hidden-form" id="getIDB">
            <input
              name="iDB"
              value="%iDB%"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              class="table_input"
              onchange="handle_onchange_and_reload('getIDB')"
              oninput="valueIDB.innerHTML=iDB.value + ' &#0037;'"
            />
          </form>
        </td>
        <td class="right" id="valueIDB">%iDB% &#0037;</td>
      </tr>
      <tr %hide_blink%>
        <td class="left">Blink:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIDBlink">
            <select
              name="input_daytime_blink"
              class="table_input"
              onchange="handle_onchange_and_reload('getIDBlink')"
            >
              %input_daytime_blink%
            </select>
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td><hr /></td>
        <td><hr /></td>
        <td><hr /></td>
      </tr>
      <!----------------------------------------------------->
      <!-- Sleep Time  -->

      <tr>
        <td class="left">Schlafenszeit:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getIST">
            <input
              type="time"
              name="iST"
              value="%iST%"
              class="table_input"
              onchange="handle_onchange_and_reload('getIST')"
            />
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td class="left">Lichtmodus:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getISM">
            <select
              name="iSM"
              class="table_input"
              onchange="handle_onchange_and_reload('getISM')"
            >
              %iSM%
            </select>
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr %hide_sleeptime_color%>
        <td class="left">Farbe:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getISC">
            <input
              type="color"
              name="iSC"
              value="%iSC%"
              class="table_input"
              onchange="handle_onchange_and_reload('getISC')"
            />
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td class="left">Helligkeit:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getISB">
            <input
              name="iSB"
              value="%iSB%"
              type="range"
              orient="vertical"
              min="0"
              max="100"
              class="table_input"
              onchange="handle_onchange_and_reload('getISB')"
              oninput="valueISB.innerHTML=iSB.value + ' &#0037;'"
            />
          </form>
        </td>
        <td class="right" id="valueISB">%iSB% &#0037;</td>
      </tr>
      <tr %hide_blink%>
        <td class="left">Blink:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getISBlink">
            <select
              name="input_sleep_blink"
              class="table_input"
              onchange="handle_onchange_and_reload('getISBlink')"
            >
              %input_sleep_blink%
            </select>
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td><hr /></td>
        <td><hr /></td>
        <td><hr /></td>
      </tr>
      <!----------------------------------------------------->
      <!-- Blink Time -->
      <tr %hide_blink%>
        <td class="left">Blink Intervall:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getBlink">
            <input
              name="iBT"
              value="%iBT%"
              type="range"
              orient="vertical"
              min="500"
              max="5000"
              step="500"
              class="table_input"
              onchange="handle_onchange_and_reload('getBlink')"
              oninput="valueIBT.innerHTML=iBT.value + ' ms;'"
            />
          </form>
        </td>
        <td class="right" id="valueIBT">%iBT% ms</td>
      </tr>
      <tr %hide_blink%>
        <td><hr /></td>
        <td><hr /></td>
        <td><hr /></td>
      </tr>
      <!----------------------------------------------------->
      <!-- Time Zone -->
      <tr>
        <td class="left">Zeitzone:</td>
        <td class="middle">
          <form action="/get" target="hidden-form" id="getTZ">
            <select
              name="input_timezone"
              class="table_input"
              onchange="handle_onchange_and_reload('getTZ')"
            >
              %input_timezone%
            </select>
          </form>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td><hr /></td>
        <td><hr /></td>
        <td><hr /></td>
      </tr>
      <tr>
        <td class="left">
          Uhrzeit:<br />
          <label style="font-size: 0.75em"> (der Lampe)</label>
        </td>
        <td class="middle">
          %input_time_on_load%
          <br />
          <label style="font-size: 0.75em"> (wird nicht aktualisiert)</label>
        </td>
        <td class="right"></td>
      </tr>
      <tr>
        <td><hr /></td>
        <td><hr /></td>
        <td><hr /></td>
      </tr>
    </table>
    <a href="/"><br />Zur Startseite</a>
    <br />
    <br />
    <br />
    <br />
    <iframe style="display: none" name="hidden-form"></iframe>
  </body>
</html>
)rawliteral";
