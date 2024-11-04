# README

Este repositorio contiene tres ejemplos de programas en C que utilizan hilos y sincronización para realizar diferentes tareas. Los programas están diseñados para ilustrar conceptos básicos de programación concurrente con Pthreads.

## 1. Monte Carlo Pi Estimation

### Descripción
Este programa utiliza el método de Monte Carlo para estimar el valor de π. Genera puntos aleatorios dentro de un cuadrado y cuenta cuántos caen dentro de un cuarto de círculo inscrito.

## 2. Medición del Tiempo de Creación de Hilos

### Descripción
Este programa mide el tiempo promedio que toma crear y terminar un hilo en el sistema. Se crea un número especificado de hilos y se mide el tiempo que tarda cada operación de creación y terminación, proporcionando así una visión de la eficiencia del manejo de hilos en el entorno de ejecución.

## 3. Candado de Lectura-Escritura Personalizado con Lista Enlazada

### Descripción
Este programa implementa una lista enlazada y utiliza un candado de lectura-escritura personalizado para permitir el acceso concurrente seguro a la lista. Permite múltiples lectores o un único escritor a la vez, asegurando que la lectura y escritura en la lista enlazada se realicen de manera segura y eficiente en un entorno multihilo.
