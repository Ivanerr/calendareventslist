# calendareventslist
A fully working watchface for Pebble Time with access to Google Calendar.
Based on the original code at https://gist.github.com/zostay/9684420 by Zostay https://gist.github.com/zostay

Written for my own pleasure and I like ugly code :)

1. You'll have to replace 
CATCHAUTH_URL
CONFIG_URL
with your own links to your files in configuration.html and catchauth.html
You'll have to host these files somewhere (I just use dropbox).
Also change all the links you'll find in the other files to links to your own files.

2. You'll have to change
var GOOGLE_CLIENT_ID = "xxxxxxx.apps.googleusercontent.com";
var GOOGLE_CLIENT_SECRET = "xxxxxxxxxxxxx"; 
in weather.js
to your own data obtained from google ->
Create a project in google developer console, go to APIs and Auth there,
go to Credentials and get a new OAuth 2.0 client ID (as a Web Application).
