echo 'const char index_html[] PROGMEM = R"rawliteral(' > tmp_index.html.h
cat index.html >> tmp_index.html.h
echo ')rawliteral";' >> tmp_index.html.h
mv tmp_index.html.h ./neolamp/src/html/index.html.h

echo 'const char settings_html[] PROGMEM = R"rawliteral(' > tmp_settings.html.h
cat settings.html >> tmp_settings.html.h
echo ')rawliteral";' >> tmp_settings.html.h
mv tmp_settings.html.h ./neolamp/src/html/settings.html.h

echo 'const char timer_html[] PROGMEM = R"rawliteral(' > tmp_timer.html.h
cat timer.html >> tmp_timer.html.h
echo ')rawliteral";' >> tmp_timer.html.h
mv tmp_timer.html.h ./neolamp/src/html/timer.html.h
