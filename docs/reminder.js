var particle = new Particle();
var token;
var device;
var eventStream;
var $app = $("#app");

var codeUrl = "https://raw.githubusercontent.com/carloop/app-reminder/master/src";
var appFiles = [
  "app-reminder.cpp",
  "project.properties",
];

var templates = {
  loginForm: _.template($("#template-login-form").text()),
  selectDevice: _.template($("#template-select-device").text()),
  error: _.template($("#template-error").text()),
  mainUI: _.template($("#main-ui").text()),
};

function getToken() {
  token = localStorage.getItem('particle-token');
  return token;
}

function setToken(newToken) {
  token = newToken;
  localStorage.setItem('particle-token', token);
}

function getDevice() {
  device = localStorage.getItem('particle-device');
  return device;
}

function setDevice(newDevice) {
  device = newDevice;
  localStorage.setItem('particle-device', device);
}


function login() {
  if (!getToken()) {
    $app.html(templates.loginForm());
    $('#login-form').on('submit', function (event) {
      event.preventDefault();
      particleLogin()
    });
  } else {
    selectDeviceForm();
  }
}

function particleLogin() {
  var $username = $('#username');
  var $password = $('#password');
  var $submit = $('#login');
  var $errorMessage = $('#error-message');

  $username.prop('disabled', true);
  $password.prop('disabled', true);
  $submit.prop('disabled', true);

  var username = $username.val();
  var password = $password.val();

  particle.login({ username: username, password: password })
  .then(function (data) {
    setToken(data.body.access_token);
    selectDeviceForm();
  }, function (err) {
    var message = err.body && err.body.error_description || "User credentials are invalid";
    $errorMessage.html(message);

    $username.prop('disabled', false);
    $password.prop('disabled', false);
    $submit.prop('disabled', false);
  });
}

function selectDeviceForm(force) {
  if (force || !getDevice()) {
    particle.listDevices({ auth: token })
    .then(function (data) {
      var devices = data.body;
      $app.html(templates.selectDevice({ devices: devices}));
      $('[data-toggle="select"]').select2();
      
      $("#select-device").on("submit", function (event) {
        event.preventDefault();
        setDevice($("#device").val());
        mainUI();
      });
    })
    .catch(function (err) {
      showError();
    });
  } else {
    mainUI();
  }
}

function mainUI() {
  $app.html(templates.mainUI());

  var $flashButton = $("#flash-button");
  var $readButton = $("#read-codes");
  var $clearButton = $("#clear-codes");
  var $logoutButton = $("#logout-button");
  $flashButton.on('click', flashApp);
  $logoutButton.on('click', logout);

  readMileage();
}

function timeoutPromise(ms) {
  return new Promise(function (fulfill, reject) {
    setTimeout(function () {
      reject(new Error("Timeout"));
    }, ms);
  });
}

function flashApp() {
  clearConsole();
  log("Start flashing...");
  var files = {};

  var filePromises = appFiles.map(function (f) {
    return $.ajax(codeUrl + "/" + f)
    .then(function (data) {
      files[f] = new Blob([data], { type: "text/plain" });
    });
  });

  Promise.all(filePromises)
  .then(function () {
    var flashPromise = particle.flashDevice({
      deviceId: device,
      files: files,
      auth: token
    });

    // Add timeout to flash
    return Promise.race([flashPromise, timeoutPromise(15000)]);
  })
  .then(function (data) {
    var body = data.body;

    if (body.ok) {
      setTimeout(function () {
        log("Done flashing!");
      }, 2000);
    } else {
      error("Error during flash.");
      error(body.errors.join("\n"));
    }
  }, function (err) {
    if (err.message == "Timeout") {
      error("Timeout during flash. Is your device connected to the Internet (breathing cyan)?");
      return;
    }

    throw err;
  })
  .catch(function (err) {
    console.error(err);
    showError();
  });
}

function readMileage() {
  clearConsole();
  log("Reading mileage...");

  var callPromise = particle.getVariable({
    deviceId: device,
    name: 'count',
    auth: token
  });

  // Add timeout to function call
  Promise.race([callPromise, timeoutPromise(10000)])
  .then(function (data) {
    if (data && data.body && data.body.result) {
      var mileage = data.body.result;
      log("Done.");
      updateMileage(mileage);
    } else {
      throw new Error('Unexpected response');
    }
  })
  .catch(function (err) {
    if (err.message === "Timeout") {
      error("Timeout. Is your device connected to the Internet (breathing cyan)?");
      return;
    }
    if (err.statusCode === 404) {
      error("Your Carloop is not yet running this app. Hit the flash button to get started.");
      return;
    }
    console.error(err);
    error("Error while reading mileage. Try flashing the app to your Carloop.");
  });
}

function updateMileage(mileage) {
  mileage = mileage || 0;
  var $el = $("#current-mileage");
  var displayMileage = Math.round(mileage*10)/10;
  $el.html(displayMileage + " miles");
}

function log(message) {
  printToConsole(message, 'info');
}

function error(message) {
  printToConsole(message, 'error');
}

function printToConsole(message, type, rawHtml) {
  var $el = $('<div class="' + type + '"/>');
  if (rawHtml) {
    $el.html(message);
  } else {
    $el.text(message);
  }
  $("#console").append($el);
}

function clearConsole() {
  $("#console").html('');
}

function showError() {
  $app.html(templates.error());
}

function logout() {
  setToken('');
  setDevice('');
  window.location.reload();
}


login();
