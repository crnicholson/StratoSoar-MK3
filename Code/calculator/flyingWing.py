import math

# Settings for the flying wing.
wcl = 7  # Wing cube loading of the glider.
ar = 6  # Aspect ratio fo the wing.
taper = 0.45  # Taper ratio of the wing.
weight = 200  # Weight in grams of the projected glider.
sweep = 25 # Sweep angle of the wing.

sa = (weight / wcl) ** (1 / 1.5)

saMM = sa * 10000

span = (ar * saMM) ** (1 / 2)

chord = span / ar

tipChord = chord * (taper*2)

rootChord = chord * (taper/2 + 1)

print("SA:", sa, "dm^2")
print("Span:", span, "mm")
print("Chord:", chord, "mm")
print("Tip chord:", tipChord, "mm")
print("Root chord:", rootChord, "mm")
print("Sweep:", sweep, "degrees")
