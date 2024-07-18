import svg_turtle
import math

# padding = 5  # Padding for the SVG.

# Settings for the flying wing.
airfoilTopLength = 190.9
wcl = 7  # Wing cube loading of the glider.
ar = 6  # Aspect ratio fo the wing.
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

print("Surface area:", sa, "dm^2")
print("Span:", span, "mm")
print("Chord:", chord, "mm")
print("Tip chord:", tipChord, "mm")
print("Root chord:", rootChord, "mm")
print("Fin surface area:", finSA, "mm^2")
print("Fin chord:", finChord, "mm")
print("Fin height:", finHeight, "mm")
print("Sweep:", sweep, "degrees")

halfSpan = span / 2

le = halfSpan / math.cos(math.radians(sweep))
offset = halfSpan * math.tan(math.radians(sweep))

width = airfoilTopLength * math.sin(math.radians(180 - sweep * 2))
height = math.sqrt(airfoilTopLength**2 - width**2)

print("Height", height, "mm")
print("Width", width, "mm")

turtle = svg_turtle.SvgTurtle(800, 600)

# turtle.mode("logo")

# turtle.penup()
# turtle.goto(0, offset)
# turtle.pendown()
# turtle.setheading(sweep)
# turtle.forward(le)

# turtle.penup()
# turtle.goto(0, offset+tipChord)
# turtle.pendown()
# turtle.setheading(0)
# turtle.forward(tipChord)

turtle.penup()
turtle.goto(0, 0)
turtle.pendown()
# turtle.setheading(sweep)
turtle.forward(100)

svg_output = turtle.to_svg()

with open("flying_wing.svg", "w") as file:
    file.write(svg_output)
