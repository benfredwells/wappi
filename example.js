// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Called by the common.js module.
function attachListeners() {
}

// Called by the common.js module.
function moduleDidLoad() {
  // The module is not hidden by default so we can easily see if the plugin
  // failed to load.
  common.hideModule();
  setupAudio();
}

function sendBuffer(buffer) {
  common.naclModule.postMessage('BF');
  common.naclModule.postMessage(buffer);
}

var kStartQueueSize = 1;
var gReceivedBuffer = [];
// Called by the common.js module.
function handleMessage(e) {
  gReceivedBuffer.push(e.data);

}

var gContext = null;

function setupAudio() {
  gContext = new webkitAudioContext();

  var compressor = gContext.createDynamicsCompressor();
  var gainNode = gContext.createGain();
  gainNode.gain.value = 1;

  // Create a streaming audio source.
  var audioElement = document.getElementById('mp3');
  var streamingAudioSource = gContext.createMediaElementSource(audioElement);

  var scriptProcessor = gContext.createScriptProcessor(512, 1, 1);
  streamingAudioSource.connect(scriptProcessor);
  scriptProcessor.connect(gainNode);
  //streamingAudioSource.connect(gainNode);

  gainNode.connect(compressor);
  compressor.connect(gContext.destination);

  scriptProcessor.onaudioprocess = handleAudio;
}

var gStarted = false;
function handleAudio(audioEvent) {
  var inputArray = audioEvent.inputBuffer.getChannelData(0);
  sendBuffer(inputArray.buffer)

  if (gStarted || gReceivedBuffer.length >= kStartQueueSize) {
    gStarted = true;
    var naclBuffer = new Float32Array(gReceivedBuffer.shift());
    var outputArray = audioEvent.outputBuffer.getChannelData(0);
    for (var i = 0; i < naclBuffer.length; i++)
      outputArray[i] = naclBuffer[i];
  }
}