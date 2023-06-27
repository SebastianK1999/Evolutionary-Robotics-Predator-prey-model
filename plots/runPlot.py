import os

import plotly.graph_objects as go
import pandas as pd
import numpy as np

SKIP_IF_EXISTS = True
MARKER_SIZE = 15
FONT_SIZE = 18

allMapWalls = {
    "empty": {
        "size": 30,
        "walls": []
    },
    "scenario_0": {
        "size": 20,
        "walls": [
            {"x":-8 ,"y":-8, "size": 2},
            {"x":-8 ,"y": 8, "size": 2},
            {"x": 8 ,"y":-8, "size": 2},
            {"x": 8 ,"y": 8, "size": 2},
        ]
    },
    "scenario_1": {
        "size": 20,
        "walls": [
            {"x":-10 ,"y":  0, "size": 2},
            {"x": 10 ,"y":  0, "size": 2},
            {"x":  0 ,"y":-10, "size": 2},
            {"x":  0 ,"y": 10, "size": 2},
        ]
    },
    "scenario_2": {
        "size": 20,
        "walls": [
            {"x":  0 ,"y":0, "size": 2},
            {"x":-10 ,"y":0, "size": 2},
            {"x": 10 ,"y":0, "size": 2},
        ]
    },
    "scenario_3": {
        "size": 20,
        "walls": [
            {"x":  0 ,"y":  0, "size": 4},
            {"x":-18 ,"y":-18, "size": 2},
            {"x":-18 ,"y": 18, "size": 2},
            {"x": 18 ,"y":-18, "size": 2},
            {"x": 18 ,"y": 18, "size": 2},
        ]
    },
    "scenario_4": {
        "size": 20,
        "walls": [
            {"x": 18 ,"y":  0, "size": 2},
            {"x": 16 ,"y":  0, "size": 2},
            {"x":-18 ,"y":  0, "size": 2},
            {"x":-16 ,"y":  0, "size": 2},
            {"x":  0 ,"y": 18, "size": 2},
            {"x":  0 ,"y": 16, "size": 2},
            {"x":  0 ,"y":-18, "size": 2},
            {"x":  0 ,"y":-16, "size": 2},
        ]
    },
}

def plotWall(fig, wall):
    fig.add_trace ( go.Scatter (
        x=[wall["x"] - wall["size"] ,wall["x"] + wall["size"], wall["x"] + wall["size"], wall["x"] - wall["size"]],
        y=[wall["y"] + wall["size"] ,wall["y"] + wall["size"], wall["y"] - wall["size"], wall["y"] - wall["size"]],
        fill="toself"))

def plotWalls(fig, walls):
    xs = []
    ys = []
    for wall in walls:
        xs = xs + [wall["x"] - wall["size"], wall["x"] + wall["size"], wall["x"] + wall["size"], wall["x"] - wall["size"], wall["x"] - wall["size"], None]
        ys = ys + [wall["y"] + wall["size"], wall["y"] + wall["size"], wall["y"] - wall["size"], wall["y"] - wall["size"], wall["y"] + wall["size"], None]
    xs = [x/2 if x != None else None  for x in xs]
    ys = [y/2 if y != None else None  for y in ys]  
    fig.add_trace ( go.Scatter (
        x=xs,
        y=ys,
        fill="toself",
        name="Przeszkody",))

def getMapName(file):
    return file[file.index("(") +1: file.index(")")]

def openFile(file):
    data = pd.DataFrame(np.loadtxt(file))
    return data

def createPositionPlot(dir, fileSuffix, predData, preyData, currentWalls, currentSize):
    fig = go.Figure()
    plotWalls(fig, currentWalls)
    fig.add_trace ( go.Scatter (
        x=[x/2 for x in predData[0]],
        y=[y/2 for y in predData[1]],
        mode="lines+markers",
        marker=dict(
            symbol="arrow",
            size=MARKER_SIZE,
            angleref="previous",
        ),
        name="Przebyta ścieżka łowcy",
    ))
    fig.add_trace ( go.Scatter (
        x=[x/2 for x in preyData[0]],
        y=[y/2 for y in preyData[1]],
        mode="lines+markers",
        marker=dict(
            symbol="arrow",
            size=MARKER_SIZE,
            angleref="previous",
        ),
        name="Przebyta ścieżka ofiary",
    ))

    layout = go.Layout(
        xaxis_title = "x [Średnice robota]",
        yaxis_title = "y [Średnice robota]",
        xaxis=go.XAxis(
            range=[-currentSize/2, currentSize/2],
        ),
        yaxis=go.YAxis(
            range=[-currentSize/2, currentSize/2],
        ),
        height=800,
        width=1050,
        font=dict(
            family="Helvet, monospace",
            size=FONT_SIZE
        )
    )
    fig.update_layout(layout)
    if not os.path.exists(str(dir)+"pos"+fileSuffix) or not SKIP_IF_EXISTS:
        fig.write_image(str(dir)+"pos"+fileSuffix)
    # print( fileSuffix )
    # fig.show()

def createDistancePlot(dir, fileSuffix, predData, timeDur):
    fig = go.Figure()
    fig.add_trace ( go.Scatter (
        x=[i * timeDur/2 for i in range(len(predData[0]))],
        y=predData[3],
        name="Dystans między robotami",
        marker=dict(
            symbol="x",
            size=MARKER_SIZE,
            angleref="previous",
        ),
    ))

    layout = go.Layout(
        xaxis_title = "Czas symulacji [s]",
        yaxis_title = "Dystans [Średnice robota]",
        height=800,
        width=1200,
        font=dict(
            family="Helvet, monospace",
            size=FONT_SIZE
        )
    )
    fig.update_layout(layout)
    # fig.update_xaxes(range=[0, 9])
    fig.update_yaxes(range=[0, 30]) 
    if not os.path.exists(str(dir)+"dist"+fileSuffix) or not SKIP_IF_EXISTS:
        fig.write_image(str(dir)+"dist"+fileSuffix)

def createDriverPlot(dir, fileSuffix, predData, preyData, timeDur):
    fig = go.Figure()
    fig.add_trace ( go.Scatter (
        x=[i * timeDur for i in range(len(predData[0]))],
        y=predData[4],
        name="Łowca - lewy silnik",
        marker=dict(
            symbol="x",
            size=MARKER_SIZE,
            angleref="previous",
        ),
    ))
    fig.add_trace ( go.Scatter (
        x=[i * timeDur for i in range(len(predData[0]))],
        y=predData[5],
        name="Łowca - prawy silnik",
        marker=dict(
            symbol="x",
            size=MARKER_SIZE,
            angleref="previous",
        ),
    ))

    fig.add_trace ( go.Scatter (
        x=[i * timeDur for i in range(len(preyData[0]))],
        y=preyData[4],
        name="Ofiara - lewy silnik",
        marker=dict(
            symbol="x",
            size=MARKER_SIZE,
            angleref="previous",
        ),
    ))
    fig.add_trace ( go.Scatter (
        x=[i * timeDur for i in range(len(preyData[0]))],
        y=preyData[5],
        name="Ofiara - prawy silnik",
        marker=dict(
            symbol="x",
            size=MARKER_SIZE,
            angleref="previous",
        ),
    ))

    layout = go.Layout(
        xaxis_title = "Czas symulacji [s]",
        yaxis_title = "Sygnał sterownika",
        height=800,
        width=1200,
        font=dict(
            family="Helvet, monospace",
            size=FONT_SIZE
        )
    )
    fig.update_yaxes(range=[-1.1, 1.1]) 
    fig.update_layout(layout)
    if not os.path.exists(str(dir)+"driv"+fileSuffix) or not SKIP_IF_EXISTS:
        fig.write_image(str(dir)+"driv"+fileSuffix)

def createPlots(file, sourceRoot: str, targetRoot: str):
    if os.stat(file).st_size == 0:
        print("Empty",file)
        return
    mapName = getMapName(file)
    currentWalls = allMapWalls[mapName]["walls"]
    currentSize = allMapWalls[mapName]["size"]
    predData = openFile(file)
    preyData = openFile(file.replace("predator", "prey"))
    if len(predData.keys()) < 6:
        return

    sourceExtension = ".txt"
    targetExtension = ".png"
    file = file[len(sourceRoot):-len(sourceExtension)]
    targetFile = targetRoot + file + targetExtension
    dir = os.path.splitext(str(targetFile).replace("predator_",""))[0] 
    fileSuffix = "_" + os.path.split(str(dir))[1] +"_"+ os.path.split(os.path.split(str(dir))[0])[1] + ".png"
    dir = dir + "/"
    createPositionPlot(dir, fileSuffix, predData, preyData, currentWalls, currentSize)
    createDistancePlot(dir, fileSuffix, predData, 0.05)
    createDriverPlot(dir, fileSuffix, predData, preyData, 0.05)


