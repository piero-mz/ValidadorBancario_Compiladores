# Sistema de Validación de Datos Bancarios
**Curso:** Compiladores — USIL 2026-1  
**Caso 2:** Analizador Léxico y Sintáctico

---

## Descripción

Este programa implementa un **analizador léxico y sintáctico** para validar datos bancarios ingresados por el usuario. El sistema procesa la entrada en dos etapas:

1. **Analizador léxico:** recorre la cadena carácter por carácter usando un Autómata Finito Determinista (AFD) y clasifica cada elemento en tokens (PALABRA_CLAVE, NUMERO_CUENTA, MONTO, FECHA, etc.).
2. **Analizador sintáctico:** verifica que la secuencia de tokens siga las reglas gramaticales definidas para cada campo bancario. Si algún campo no cumple el formato esperado, el sistema reporta el error exacto.

Solo si **ambas validaciones** son exitosas, los datos se muestran como registrados correctamente.

---

## Compilación

```bash
g++ -std=c++17 -o ValidadorBancario ValidadorBancario.cpp
./ValidadorBancario
```

---

## Formato de entrada

Cada campo debe seguir la estructura:

```
PALABRA_CLAVE: valor;
```

| Campo     | Tipo esperado       | Formato / Regla                        | Ejemplo                  |
|-----------|---------------------|----------------------------------------|--------------------------|
| CUENTA    | NUMERO_CUENTA       | 4 dígitos - 4 dígitos - 4 dígitos      | `CUENTA: 4920-1234-5678;`|
| TITULAR   | TEXTO               | Letras y espacios                      | `TITULAR: Juan Perez;`   |
| TIPO      | TIPO_CUENTA         | Solo AHORROS o CORRIENTE               | `TIPO: AHORROS;`         |
| SALDO     | MONTO               | Número decimal positivo                | `SALDO: 1500.50;`        |
| FECHA     | FECHA               | Formato AAAA-MM-DD                     | `FECHA: 2026-04-26;`     |
| CODIGO    | NUMERO              | Número entero positivo                 | `CODIGO: 78901;`         |

---

## Ejemplo de input válido

```
CUENTA: 4920-1234-5678;
TITULAR: Juan Perez;
TIPO: AHORROS;
SALDO: 1500.50;
FECHA: 2026-04-26;
CODIGO: 78901;
```
*(Presionar Enter en línea vacía para terminar)*

## Ejemplo de input inválido

```
CUENTA: 4920-1234;
TITULAR: Ana Lopez;
TIPO: VISA;
SALDO: abc;
```

---

## Tokens reconocidos

| Token          | Descripción                        | Ejemplo           |
|----------------|------------------------------------|-------------------|
| PALABRA_CLAVE  | Campo del formulario               | CUENTA, SALDO     |
| SEPARADOR      | Dos puntos entre campo y valor     | `:`               |
| FIN_CAMPO      | Punto y coma que cierra el campo   | `;`               |
| NUMERO_CUENTA  | Número de cuenta bancaria          | 4920-1234-5678    |
| MONTO          | Valor monetario decimal            | 1500.50           |
| FECHA          | Fecha en formato ISO               | 2026-04-26        |
| TEXTO          | Nombre o texto libre               | Juan Perez        |
| TIPO_CUENTA    | Tipo de cuenta bancaria            | AHORROS           |
| NUMERO         | Entero positivo                    | 78901             |
| ERROR          | Carácter o valor no reconocido     | @, #, abc         |
