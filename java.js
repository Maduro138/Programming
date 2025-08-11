console.log("Hello World!");


 let leticia= 34;
  
   let leticia2 = parsentInt(prompt("Digite um numero"));
   let leticia3 = parsentInt(prompt("Digite outro numero"));
   if (leticia2 > leticia3) {
     alert("O número"+ leticia2 "é maior que o" + leticia3 );
        
   } else if {
     alert("O número"+ leticia3 "é maior que o" + leticia2 );
   }

else {
    alert("os numeros são iguais");
}




   function adicionar(valor){
       let tela = document.getElementById('tela');
       tela.value += valor;
   } 

  function calcular(){
   /* let tela = document.getElementById("tela");
    tela.value = eval(tela.value);
  */
    try {

     const result = eval( document.getElementById('tela').value);
      document.getElementById('tela').value = result;
       
    } catch (e) {
      alert("Erro na expressão matemática");
      
    }
   }

  function limpar(){
    let tela = document.getElementById('tela');
    tela.value = '';
  }
