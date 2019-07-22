# -*- coding: utf-8 -*-
#!/usr/bin/env python 
#coding=utf-8
import time
import json
import os
import speech_recognition as sr
from espeak import espeak
from lib import qiot

"""
    requirement:
    -- pip install paho-mqtt
    run command: python mqtt.py
"""

"""
    Setup connection options
"""
connection = None
connection = qiot.connection(qiot.protocol.MQTT)
connection_options = connection.read_resource('./res/resourceinfo.json', '/ssl/')

"""
    Send data to QIoT Suite Lite.
"""

r = sr.Recognizer()
sr.operation_timeout = 5
#sr.dynamic_energy_threshold = True

def speak_text(text):
	command_speak = 'espeak -s 200 -v en+f3 \"' + text +'\"'
	os.system(command_speak)

def speech(audio_in):
    try:
        text = r.recognize_google(
            audio_in, key=None, language="en-US", show_all=False)
        print("Google Speech Recognition thinks you said " + text)
        connection.publish_by_id("echotext", text)
    except sr.UnknownValueError:
        print("Google Speech Recognition could not understand audio")
    except sr.RequestError as e:
        print("Could not request results from Google Speech Recognition service; {0}".format(e))

def on_message(event_trigger, data):
    message = json.loads(data["message"].payload)
    if(data['id'] == 'action'):
        print( "action : " + str(message['value']))
        print( "------------------------")
        if(message['value'] == 1):
            with sr.Microphone() as source:
                print("Say something!")
                audio = r.listen(source)
                speech(audio)
    if(data['id'] == 'playMusic'):
        print( "playMusic : " + str(message['value']))
        print( "------------------------")
        if(message['value']['command'] == "play"):
            play_music('/Multimedia/test2.mp3')
        if(message['value']['command'] == "stop"):
            stop_music()
    if(data['id'] == 'speak'):
        print("speak : " + message['value'])
        print( "------------------------")
        speak_text(message['value'].encode('utf8'))

def on_connect(event_trigger, data):
    print("client ready")
    connection.subscribe_by_id("action")
    connection.subscribe_by_id("echotext")
    connection.subscribe_by_id("speak")

connection.on("connect", on_connect)
connection.on("message", on_message)
connection.connect(connection_options)

while 1:
    pass
    time.sleep(0.1)