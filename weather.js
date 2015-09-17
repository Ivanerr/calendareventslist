
var lastrequestresponsetime=0;

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
//    console.log('PebbleKit JS ready!');
//    console.log('Doing google api!');
    do_google_api();
  }
);

// Listen for when an AppMessage is received
/*Pebble.addEventListener('appmessage',
  function(e) {
        console.log('AppMessage received!');
  }                     
);
*/
var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.timeout = 10000;
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};


function locationSuccess(pos) {
    // Construct URL
  
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
  pos.coords.latitude + '&lon=' + pos.coords.longitude + '&lang=ru';
  //console.log(url);
  // Send request to OpenWeatherMap
  
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.main.temp - 273.15);
      //console.log('Temperature is ' + temperature);
      
      var place_name = json.name;
      //console.log('Place name is ' + place_name);

      // Conditions
      var conditions = json.weather[0].main;      
     // console.log('Conditions are ' + conditions);
    
      // Assemble dictionary using our keys
      var dictionary = {
        'KEY_TEMPERATURE': temperature,
        'KEY_CONDITIONS': conditions,
        'PLACE_NAME': place_name        
      };
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
//          console.log('Weather info sent to Pebble successfully!');
        },
        function(e) {
//          console.log('Error sending weather info to Pebble!');
        }
      );
    
    }      
  );
  // We will request the weather here
}

function locationError(err) {
  ///console.log('Error requesting location!');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
/*Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');

    // Get the initial weather
    //getWeather();
    
  }
);
*/
//NEW 
// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    
    //console.log('AppMessage received!');
    //console.log('Received message: ' + JSON.stringify(e.payload));
    do_google_api();
   // getWeather();
  }                     
);

//google auth starts
//REPLACE  IT WITH YOUR DATA
var GOOGLE_CLIENT_ID = "xxxxxxx.apps.googleusercontent.com";
var GOOGLE_CLIENT_SECRET = "xxxxxxxxxxxxx";
 
// Retrieves the refresh_token and access_token.
// - code - the authorization code from Google.
function resolve_tokens(code) {
    var req = new XMLHttpRequest();
    req.timeout = 10000;
    req.open("POST", "https://accounts.google.com/o/oauth2/token", false);
    req.onload = function(e) {
        var db = window.localStorage;
        if (req.readyState == 4 && req.status == 200) {
            var result = JSON.parse(req.responseText);
 
            if (result.refresh_token && result.access_token) {
                db.setItem("refresh_token", result.refresh_token);
                db.setItem("access_token", result.access_token);
                //console.log("TOKENS SET!");
                //console.log("REFRESH:"+result.refresh_token);
                //console.log("ACCESS:"+result.access_token);
                //console.log("calling calendar from resolve_tokens");
                calendar(result.access_token);
                return;
            }
            else
            {
               //console.log("TOKENS WAS NOT RETURNED");
            }
        }
 
        db.removeItem("code");
        db.setItem("code_error", "Unable to verify the your Google authentication.");
        //console.log("UNABLE TO VERIFY GOOGLE AUTHENTICATION");
       //console.log(req.responseText);
    };
  
  //console.log("SEDING REQ.SEND");
  
  //console.log("grant_type=authorization_code"+"&code="+encodeURIComponent(code)
         //   +"&client_id="+GOOGLE_CLIENT_ID
           // +"&client_secret="+GOOGLE_CLIENT_SECRET
           // +"&redirect_uri=https://dl.dropboxusercontent.com/s/14rkuevy1zes8pv/catchauth.html"
           // +"&grant_type=authorization_code");
    req.setRequestHeader('content-type', 'application/x-www-form-urlencoded');
    req.send("grant_type=authorization_code"+"&code="+encodeURIComponent(code)
            +"&client_id="+GOOGLE_CLIENT_ID
            +"&client_secret="+GOOGLE_CLIENT_SECRET
            +"&redirect_uri=https://dl.dropboxusercontent.com/s/14rkuevy1zes8pv/catchauth.html"
            +"");
}
 
// Runs some code after validating and possibly refreshing the access_token.
// - code - code to run with the access_token, called like code(access_token)
function use_access_token(code) {
    var db = window.localStorage;
    var refresh_token = db.getItem("refresh_token");
    var access_token = db.getItem("access_token");
 
  if (!access_token) {
    //console.log("ACCESS_TOKEN NOT SET");
  //  return;
  }
  
  if (!refresh_token) {
        //console.log("REFRESH TOKEN NOT SET");
    return;
  }
  
 //console.log("launching VALID_TOKEN");
    valid_token(access_token, code, function() {
        refresh_access_token(refresh_token, code);
    });
}
 
// Validates the access token.
// - access_token - the access_token to validate
// - good - the code to run when the access_token is good, run like good(access_token)
// - bad - the code to run when the access_token is expired, run like bad()
function valid_token(access_token, good, bad) {
    var req = new XMLHttpRequest();
    req.timeout = 10000;
    //console.log("trying to open url"+"https://www.googleapis.com/oauth2/v1/tokeninfo?access_token=" + access_token);
    req.open("GET","https://www.googleapis.com/oauth2/v1/tokeninfo?access_token=" + access_token, true);
    req.onload = function(e) {
        if (req.readyState == 4 && req.status == 200) {
            var result = JSON.parse(req.responseText);
 
            if (result.audience != GOOGLE_CLIENT_ID) {
                var db = window.localStorage;
                db.removeItem("code");
                db.removeItem("access_token");
                db.removeItem("refresh_token");
                db.setItem("code_error", "There was an error validating your Google Authentication. Please re-authorize access to your account.");
                return;
            }
          //console.log("ACCESS TOKEN IS GOOD: "+access_token);
            good(access_token);
            return;
        }
 
        //console.log("ACCESS TOKEN IS BAD ");
        bad();
    };
    req.send();
}
 
// Refresh a stale access_token.
// - refresh_token - the refresh_token to use to retreive a new access_token
// - code - code to run with the new access_token, run like code(access_token)
function refresh_access_token(refresh_token, code) {
//    console.log("REFRESH TOKEN FUNCTION STARTED");
    var req = new XMLHttpRequest();
    req.open("POST", "https://accounts.google.com/o/oauth2/token", false);
    req.timeout=10000;
    req.onload = function(e) {
        if (req.readyState == 4 && req.status == 200) {
          //console.log("got some GOOD results from refresh function token");
          //console.log(req.responseText);
            var result = JSON.parse(req.responseText);
 
            if (result.access_token) {
                var db = window.localStorage;
                db.setItem("access_token", result.access_token);
                //console.log("calling calendar from REFRESH token");
                //calendar(result.access_token);
            }
         
        }
      else
        {
           //console.log("got BAD some results from refresh function token");
           //console.log(req.responseText);
        }
    };
    req.setRequestHeader('content-type', 'application/x-www-form-urlencoded');
    req.send("grant_type=refresh_token"+"&refresh_token="+encodeURIComponent(refresh_token)
            +"&client_id="+GOOGLE_CLIENT_ID
            +"&client_secret="+GOOGLE_CLIENT_SECRET
            +"");
}
 
// Finally, execute our API calls, which will then pass messages back to the watch to show stuff
function do_google_api() {
  //console.log("TRYING TO DO GOOGLE API!!");
   use_access_token(function(access_token) {
      //console.log("HURRAY!"+code);
    
  var db = window.localStorage;
    var refresh_token = db.getItem("refresh_token");
    var access_token = db.getItem("access_token");
  //console.log("ACCESS API TOKEN :"+access_token);  
  //console.log("ACCESS REFRESH TOKEN :"+access_token);  
  if(access_token!=null)
  {
    //console.log("Calling calendar from google api");
    calendar(access_token);
  
  }
  //else
  //  console.log("please login to google api");
//console.log("ENDED GOOGLE API!!");
     });
}
 

// When you click on Settings in Pebble's phone app. Go to the configuration.html page.
function show_configuration() {
    var db = window.localStorage;
    var code = db.getItem("code");
    var code_error = db.getItem("code_error");
    db.removeItem("code_error");
 
    var json = JSON.stringify({
        "code": code,
        "code_error": code_error
    });
    
    //ONE OF THE LINKS BELOW WORKS IN THE EMULATOR AND ANOTHER IN REAL WORLD
    
  //console.log("https://dl.dropboxusercontent.com/s/e68sf89n8dpxuob/configuration.html#" + encodeURIComponent(json));
  // Pebble.openURL("https://dl.dropboxusercontent.com/s/e68sf89n8dpxuob/configuration.html#" + json); 
  
  //THE LINE BELOW WORKS IN A REAL WATCH ENVIRONMENT
  Pebble.openURL("https://dl.dropboxusercontent.com/s/e68sf89n8dpxuob/configuration.html?cfg="+encodeURIComponent(json));
 //Pebble.openURL("https://dl.dropboxusercontent.com/s/e68sf89n8dpxuob/configuration.html?cfg=" + json);
  //Pebble.openURL("http://ya.ru#" + json);
 // Pebble.openURL("https://dl.dropboxusercontent.com/s/e68sf89n8dpxuob/configuration.html#");
  //Pebble.openURL("http://www.ya.ru");
}
 
// When you click Save on the configuration.html page, recieve the configuration response here.
function webview_closed(e) {
  if (typeof e.response != 'undefined'&&e.response!="") {
    //console.log("got response");
    //console.log( JSON.stringify(e) );
  
    var json = e.response;
    //console.log(decodeURIComponent(e.response));
    var config = JSON.parse(decodeURIComponent(json));
 
    var code = config.code;
   //console.log("CONFIG.CODE = "+config.code);
    
    var db = window.localStorage;
    var old_code = db.getItem("code");
      //console.log("OLD.CODE = "+old_code);
    if (old_code != code) {
        db.setItem("code", code);
        //console.log("NEW CODE SET");
        db.removeItem("refresh_token");
        db.removeItem("access_token");
    }
    //console.log("RESOLVING TOKENS in WEBVIEW CLOSED");
    resolve_tokens(code);
  }
  else
    {
      //console.log("empty or no response");
    
  if(code!==null)
      {
       // console.log("RESOLVING TOKENS VIA CONFIG CODE NOT NULL");
      }
    }
  //console.log("DOING GOOGLE API FROM WEBVIEW_CLOSED");
  do_google_api();
}

function ISODateString(d){
 function pad(n)
  {
    return n<10 ? '0'+n : n;
  }
 return d.getUTCFullYear()+'-'
      + pad(d.getUTCMonth()+1)+'-'
      + pad(d.getUTCDate())+'T'
      + pad(d.getUTCHours())+':'
      + pad(d.getUTCMinutes())+':'
      + pad(d.getUTCSeconds())+'Z';
}



function calendar(token)
{
  var stringtosend="";
  /* var url = "https://www.googleapis.com/calendar/v3/users/me/calendarList?maxResults=10&minAccessRole=owner&showDeleted=false";*/

  var d = new Date();
  
  //console.log("current isodate:");
  //console.log(ISODateString(d));
  
  var url="https://www.googleapis.com/calendar/v3/calendars/primary/events?singleEvents=true&orderBy=startTime&timeMin="+encodeURIComponent(ISODateString(d))+"&maxResults=10&fields=items(summary,start,end)";
  //var url ="https://www.googleapis.com/calendar/v3/users/me/calendarList";
  var xhr = new XMLHttpRequest();
  xhr.open("GET",url,true);
  xhr.setRequestHeader("Authorization","Bearer "+token);
  xhr.timeout = 10000;
  xhr.onload = function()
  {    
    if(xhr.readyState==4&&xhr.status==200)
    {
      //console.log("SUCCESS!");
      //console.log("LENGTH: "+xhr.responseText.length);
      
     var json = JSON.parse(xhr.responseText);
 
      var i=0;      
      
      for (i = 0; i < json.items.length; i++) {
        
      if (typeof json.items[i].start.date!='undefined' ) {
        //console.log(i+" all day event skipping ");
        continue;
      }
        var unixtime = Date.parse(json.items[i].start.dateTime)/1000;
        
        var unixtimeend = Date.parse(json.items[i].end.dateTime)/1000;
        
        stringtosend = stringtosend + "{!}"+json.items[i].summary.substring(0,60)+"{!}"+unixtime+"{!}"+unixtimeend;
        //console.log(json.items[i].summary);                               
   
    }
      
      if(i==0)
        {
          stringtosend="{!}123454321{!}12345{!}12345";
        }
      
      // Assemble dictionary using our keys
      stringtosend=stringtosend+"";
      //console.log(stringtosend);
      var events_data_json_string = stringtosend;    
      var dictionary = {
        'EVENTS_DATA': events_data_json_string
      };
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          //console.log('EVENTS info sent to Pebble successfully!');
        },
        function(e) {
          //console.log('Error sending EVENTS info to Pebble!');
        }
      );
      
      
    }    
  };
  xhr.send();

}

// Setup the configuration events
Pebble.addEventListener("showConfiguration", show_configuration);
Pebble.addEventListener("webviewclosed", webview_closed);
