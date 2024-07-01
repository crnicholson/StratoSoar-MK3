import svg_turtle
import math

# Settings for the flying wing
wcl = 7  # Wing cube loading of the glider
ar = 6  # Aspect ratio of the wing
finAR = 2  # Aspect ratio of the fin
taper = 0.45  # Taper ratio of the wing
weight = 200  # Weight in grams of the projected glider
sweep = 25  # Sweep angle of the leading edge

# Calculations
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

# Convert degrees to radians for the sweep calculation
sweep_radians = math.radians(sweep)

# Create the SVG drawing
turtle = svg_turtle.SvgTurtle(800, 600)

# Draw the flying wing
turtle.penup()
turtle.goto(0, 0)
turtle.pendown()

# Draw left half of the wing
turtle.goto(-span / 2 * math.cos(sweep_radians), span / 2 * math.sin(sweep_radians))
turtle.goto(-span / 2, tipChord)
turtle.goto(0, rootChord)

# Draw right half of the wing
turtle.goto(span / 2, tipChord)
turtle.goto(span / 2 * math.cos(sweep_radians), span / 2 * math.sin(sweep_radians))
turtle.goto(0, 0)

# Output SVG
svg_output = turtle.to_svg()
print(svg_output)

# Save the SVG to a file
with open("flying_wing.svg", "w") as file:
    file.write(svg_output)
