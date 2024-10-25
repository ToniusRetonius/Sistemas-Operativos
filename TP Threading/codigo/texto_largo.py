import random

productos_base = [
    "heladera", "licuadora", "microondas", "tostadora", "batidora",
    "ventilador", "cafetera", "plancha", "freidora", "aspiradora",
    "televisor", "computadora", "impresora", "tablet", "smartphone",
    "refrigerador", "hornito", "robot", "lavadora", "secadora"
]

def generar_nombres_productos(cantidad=10**5):
    nombres_productos = []
    for _ in range(cantidad):
        nombre = random.choice(productos_base)

        if random.random() < 0.1:  
            nombre += random.choice([' XL', ' Pro', ' Lite', ' Plus'])
        nombres_productos.append(nombre)
    return nombres_productos

nombres = generar_nombres_productos()

with open('nombres_productos.txt', 'w') as f:
    for nombre in nombres:
        f.write(nombre.strip() + '\n')


