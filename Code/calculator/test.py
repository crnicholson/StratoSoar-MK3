import turtle
import math

# Settings for the flying wing.
padding = 5  # Padding for the SVG.
airfoilTopLength = 190.9
wcl = 7  # Wing cube loading of the glider.
ar = 6  # Aspect ratio of the wing.
finAR = 2  # Aspect ratio of the fin.
taper = 0.45  # Taper ratio of the wing.
weight = 200  # Weight in grams of the projected glider.
sweep = 25  # Sweep angle of the leading edge.

sa = (weight / wcl) ** (1 / 1.5)
saMM = sa * 10000
span = (ar * saMM) ** (1 / 2)
chord = span / ar
rootChord = (2 * chord) / (1 + taper)
tipChord = rootChord * taper
finSA = 0.1 * saMM
finChord = (finAR * finSA) ** (1 / 2)
finHeight = finChord / finAR

halfSpan = span / 2
le = halfSpan / math.cos(math.radians(sweep))
offset = halfSpan * math.tan(math.radians(sweep))
width = airfoilTopLength * math.sin(math.radians(180 - sweep * 2))
height = math.sqrt(airfoilTopLength**2 - width**2)

# Turtle setup
screen = turtle.Screen()
screen.title("Flying Wing Design")
wing = turtle.Turtle()
wing.speed(0)  # Fastest drawing speed

# Move to starting position without drawing
wing.penup()
wing.goto(-halfSpan / 2, -offset / 2 - height / 2)
wing.pendown()

# Drawing the flying wing
wing.forward(halfSpan)
wing.left(90)
wing.forward(height)
wing.left(90)
wing.forward(halfSpan)
wing.left(90)
wing.forward(height)

# Drawing the leading edge
wing.penup()
wing.goto(-halfSpan / 2, -offset / 2 - height / 2)
wing.pendown()
wing.setheading(-sweep)
wing.forward(le)

# Drawing the trailing edge
wing.penup()
wing.goto(halfSpan / 2, -offset / 2 - height / 2)
wing.pendown()
wing.setheading(-sweep)
wing.forward(le)

# Drawing the fin
wing.penup()
wing.goto(0, -offset / 2 - height / 2 + chord)
wing.pendown()
wing.setheading(90)  # Pointing upwards
wing.forward(finHeight)

wing.hideturtle()
screen.mainloop()
