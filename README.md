# tesis-microrred-dc-rl-control
# Control de Voltaje Secundario para una Microrred DC basado en Aprendizaje por Refuerzo

Simulaciones desarrolladas como anexo del proyecto de grado de 
**José Alejandro Mesa Chaves**.

## Descripción

Este repositorio contiene los programas y modelos de simulación utilizados 
en el diseño, implementación y validación de un controlador de voltaje para 
una microrred de corriente continua (MGDC) basado en aprendizaje por refuerzo.

La microrred está compuesta por múltiples convertidores DC-DC conectados en paralelo, 
con una arquitectura de control jerárquico de dos niveles:

- **Control Primario:** Droop control para reparto de potencia entre convertidores 
  sin comunicación directa.
- **Control Secundario:** Dos enfoques implementados y comparados:
  - Controlador clásico **PI** (referencia)
  - Controlador basado en **Q-Learning** (aprendizaje por refuerzo)

El agente de Q-Learning aprende una política óptima mediante interacción con 
el entorno, sin requerir un modelo explícito del sistema, bajo un enfoque 
descentralizado que reduce la dependencia de comunicación entre unidades.

## Resultados principales

| Controlador | Tiempo de establecimiento | Error estacionario | Adaptación |
|---|---|---|---|
| PI | Más lento | Bajo | Limitada |
| Q-Learning | Más rápido | Ligero incremento | Alta |

## Contenido del repositorio

- Modelos de simulación en MATLAB/Simulink
- Scripts de implementación del algoritmo Q-Learning
- Escenarios de prueba: variaciones de carga y cambios en generación
