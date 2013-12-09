chrome.app.runtime.onLaunched.addListener(function() {
  var width = 500;
  var height = 300;

  chrome.app.window.create('index.html', {
    id: "audio",
    bounds: {
      width: width,
      height: height,
    }
  });
});
