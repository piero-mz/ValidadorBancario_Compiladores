#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <regex>
#include <iomanip>
using namespace std;

// BLOQUE 1: Tipos de tokens y estructura Token
enum TokenType {
    PALABRA_CLAVE, SEPARADOR, NUMERO_CUENTA, MONTO, FECHA,
    TEXTO, TIPO_CUENTA, FIN_CAMPO, NUMERO, END, ERROR_TOKEN
};
struct Token { TokenType type; string lexema; };

// BLOQUE 2: Palabras clave
unordered_set<string> palabrasClave = {"CUENTA","TITULAR","TIPO","SALDO","FECHA","CODIGO"};
unordered_set<string> tiposCuenta   = {"AHORROS","CORRIENTE"};

// BLOQUE 3: Clasificador de caracteres
int getCharClass(char c) {
    if (isalpha(c)) return 0;
    if (isdigit(c)) return 1;
    if (c=='-')     return 2;
    if (c=='.')     return 3;
    if (c==':')     return 4;
    if (c==';')     return 5;
    if (isspace(c)) return 6;
    return 7;
}

// BLOQUE 4: Tabla AFD T[7][8]
int T[7][8] = {
    { 1, 2,-1,-1, 5, 6, 0,-1},
    { 1, 1,-1,-1,-1,-1,-1,-1},
    {-1, 2, 3, 4,-1,-1,-1,-1},
    {-1, 3, 3,-1,-1,-1,-1,-1},
    {-1, 4,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1}
};
TokenType finalState[7] = {ERROR_TOKEN,TEXTO,NUMERO,TEXTO,MONTO,SEPARADOR,FIN_CAMPO};

// BLOQUE 5: Analizador Lexico
string toUpper(const string& s){ string r=s; for(char&c:r) c=toupper(c); return r; }

vector<Token> tokenizar(const string& input) {
    vector<Token> tokens;
    int state=0; string lexema="";

    auto emitir=[&](){
        if(lexema.empty()) return;
        TokenType type=finalState[state];
        string up=toUpper(lexema);
        if(type==TEXTO){
            if(palabrasClave.count(up))   {type=PALABRA_CLAVE;lexema=up;}
            else if(tiposCuenta.count(up)){type=TIPO_CUENTA;  lexema=up;}
            else if(regex_match(lexema,regex("^[0-9]{4}-[0-9]{4}-[0-9]{4}$"))) type=NUMERO_CUENTA;
            else if(regex_match(lexema,regex("^[0-9]{4}-[0-9]{2}-[0-9]{2}$"))) type=FECHA;
        }
        tokens.push_back({type,lexema});
        lexema=""; state=0;
    };

    for(char c:input){
        int cls=getCharClass(c);
        int next=T[state][cls];
        if(next==-1){
            emitir();
            if     (c==':') tokens.push_back({SEPARADOR,":"});
            else if(c==';') tokens.push_back({FIN_CAMPO,";"});
            else if(!isspace(c)) tokens.push_back({ERROR_TOKEN,string(1,c)});
        } else {
            state=next;
            if(!isspace(c)) lexema+=c;
        }
    }
    emitir();
    tokens.push_back({END,""});
    return tokens;
}

// BLOQUE 6: Analizador Sintactico
string tokenName(TokenType t){
    switch(t){
        case PALABRA_CLAVE: return "PALABRA_CLAVE";
        case SEPARADOR:     return "SEPARADOR";
        case NUMERO_CUENTA: return "NUMERO_CUENTA";
        case MONTO:         return "MONTO";
        case FECHA:         return "FECHA";
        case TEXTO:         return "TEXTO";
        case TIPO_CUENTA:   return "TIPO_CUENTA";
        case FIN_CAMPO:     return "FIN_CAMPO";
        case NUMERO:        return "NUMERO";
        case END:           return "END";
        default:            return "ERROR";
    }
}
bool esValor(TokenType t){
    return t==NUMERO_CUENTA||t==MONTO||t==FECHA||t==TEXTO||t==TIPO_CUENTA||t==NUMERO;
}

struct Resultado { bool valido; string mensaje; vector<pair<string,string>> campos; };

Resultado validar(const vector<Token>& tokens){
    Resultado res; res.valido=true; int i=0;
    while(tokens[i].type!=END){
        if(tokens[i].type!=PALABRA_CLAVE){
            res.valido=false;
            res.mensaje="Error: se esperaba PALABRA_CLAVE, se encontro '"+tokens[i].lexema+"'";
            return res;
        }
        string campo=tokens[i].lexema; i++;

        if(tokens[i].type!=SEPARADOR){
            res.valido=false;
            res.mensaje="Error en '"+campo+"': falta ':', se encontro '"+tokens[i].lexema+"'";
            return res;
        }
        i++;

        if(!esValor(tokens[i].type)){
            res.valido=false;
            res.mensaje="Error en '"+campo+"': valor invalido '"+tokens[i].lexema
                        +"' ("+tokenName(tokens[i].type)+")";
            return res;
        }

        string valor=tokens[i].lexema; i++;
        while(tokens[i].type==TEXTO){ valor+=" "+tokens[i].lexema; i++; }

        if(campo=="CUENTA"&&!regex_match(valor,regex("^[0-9]{4}-[0-9]{4}-[0-9]{4}$"))){
            res.valido=false; res.mensaje="Error: CUENTA formato invalido '"+valor+"'"; return res;
        }
        if(campo=="SALDO"&&!regex_match(valor,regex("^[0-9]+(\\.[0-9]+)?$"))){
            res.valido=false; res.mensaje="Error: SALDO debe ser decimal positivo '"+valor+"'"; return res;
        }
        if(campo=="FECHA"&&!regex_match(valor,regex("^[0-9]{4}-[0-9]{2}-[0-9]{2}$"))){
            res.valido=false; res.mensaje="Error: FECHA formato invalido '"+valor+"'"; return res;
        }
        if(campo=="TIPO"&&!tiposCuenta.count(valor)){
            res.valido=false; res.mensaje="Error: TIPO debe ser AHORROS o CORRIENTE '"+valor+"'"; return res;
        }
        res.campos.push_back({campo,valor});

        if(tokens[i].type!=FIN_CAMPO){
            res.valido=false;
            res.mensaje="Error en '"+campo+"': falta ';', se encontro '"+tokens[i].lexema+"'";
            return res;
        }
        i++;
    }
    if(res.campos.empty()){res.valido=false;res.mensaje="Formulario vacio.";}
    else res.mensaje="Datos bancarios validos. Registro procesado correctamente.";
    return res;
}

// BLOQUE 7: main()
void sep(char c='-',int n=62){cout<<string(n,c)<<"\n";}

int main(){
    sep('=');
    cout<<"   SISTEMA DE VALIDACION DE DATOS BANCARIOS\n";
    cout<<"   Compiladores - USIL 2026-1\n";
    sep('=');
    cout<<"\nFormato esperado:\n"
        <<"  CUENTA: XXXX-XXXX-XXXX;\n"
        <<"  TITULAR: Nombre Apellido;\n"
        <<"  TIPO: AHORROS | CORRIENTE;\n"
        <<"  SALDO: 1500.50;\n"
        <<"  FECHA: AAAA-MM-DD;\n"
        <<"  CODIGO: 78901;\n\n"
        <<"Ingrese los datos (linea vacia para terminar):\n";
    sep();

    string li,input;
    while(getline(cin,li)){ if(li.empty()) break; input+=" "+li; }
    if(input.empty()){cout<<"Sin datos.\n";return 1;}

    cout<<"\n--- ANALISIS LEXICO -------------------------------------------\n";
    cout<<left<<setw(18)<<"TOKEN"<<"LEXEMA\n"; sep();
    vector<Token> tokens=tokenizar(input);
    for(auto&t:tokens){ if(t.type==END) break; cout<<left<<setw(18)<<tokenName(t.type)<<t.lexema<<"\n"; }

    cout<<"\n--- ANALISIS SINTACTICO ---------------------------------------\n";
    Resultado res=validar(tokens);
    if(res.valido){
        cout<<"RESULTADO: [VALIDO]\n\n";
        cout<<left<<setw(12)<<"CAMPO"<<"VALOR\n"; sep();
        for(auto&p:res.campos) cout<<left<<setw(12)<<p.first<<p.second<<"\n";
        sep(); cout<<res.mensaje<<"\n";
    } else {
        cout<<"RESULTADO: [INVALIDO]\n"<<res.mensaje<<"\n";
    }
    cout<<"\n"; sep('=');
    return 0;
}
