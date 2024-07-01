# Settings for the flying wing.
wcl = 7  # Wing cube loading of the glider.
ar = 6  # Aspect ratio fo the wing.
weight = 200  # Weight in grams of the projected glider.

sa = (weight / wcl) ** (1 / 1.5)

saMM = sa * 10000

span = (ar * saMM) ** (1 / 2)

chord = span / ar

print("SA:", sa, "dm^2")
print("Span:", span, "mm")
print("Chord:", chord, "mm")
