function Decoder(bytes, port) {
var obj = new Object();

switch(port) {
	
	case 1:
		// normal -- keep alive
		//Battery/ Mede a faixa de 3.6V a 3.3V de maneira acurada
		var batEncoded=(bytes[0]<<8)|(bytes[1]);
		//var batDecoded= (batEncoded*3/4096) * (78/56) +0.0093; //0.0093 compensa erro sistematico
		var batDecoded = (batEncoded*78/56)/1000;
			
		obj.Battery=batDecoded.toFixed(2) + " V";
	
		
		//Temperatura [ 2 bytes ] Sensor
		var tempEncoded=(bytes[2]<<8)|(bytes[3]);
		var axtempDecoded;

		if(tempEncoded > 0x8000){ // complemento de 2
		  axtempDecoded = tempEncoded - 65536
		}else {
		  axtempDecoded = tempEncoded;
		}
		var tempDecoded = 25.00 + (axtempDecoded/256);  
		obj.Temperatura=(parseFloat(tempDecoded)).toFixed(2) + " °C";
		
		//flag de erro 
		var flag_erro = bytes[4];
		if((flag_erro & 0x80) == 0x80) {
			erro_lum = "ALERTA";
		} else {
			erro_lum = "OK";
		}
		if ((flag_erro & 0x40) == 0x40) {
			erro_mov = "ALERTA";
		} else {
			erro_mov = "OK";
		}
		if((flag_erro & 0x20) == 0x20){
			erro_bat = "ALERTA";
		} else {
			erro_bat = "OK";
		}
		if ((flag_erro & 0x04) == 0x04) {
			erro_sensor = "ALERTA";
		} else {
			erro_sensor = "OK";
		}
		obj.statusLuminosidade = erro_lum;
		obj.statusMovimento = erro_mov;
		obj.statusBateria = erro_bat;  
		obj.statusSensor = erro_sensor;
		
	break;
	case 2:
		// alarme
	
		//flag de erro 
		flag_erro = bytes[0];
		if((flag_erro & 0x80) == 0x80) {
			erro_lum = "ALERTA";
		} else {
			erro_lum = "OK";
		}
		if ((flag_erro & 0x40) == 0x40) {
			erro_mov = "ALERTA";
		} else {
			erro_mov = "OK";
		}
		if((flag_erro & 0x20) == 0x20){
			erro_bat = "ALERTA";
		} else {
			erro_bat = "OK";
		}
		if ((flag_erro & 0x04) == 0x04) {
			erro_sensor = "ALERTA";
		} else {
			erro_sensor = "OK";
		}
		obj.statusLuminosidade = erro_lum;
		obj.statusMovimento = erro_mov;	
		obj.statusBateria = erro_bat;  
		obj.statusSensor = erro_sensor;
		
		//Luminosity Sensor
		var lumEncoded=(bytes[1]);
		if (lumEncoded >= 255) {
		  		obj.Luminosity= "Acima de 255 lx";
		} else {
  		    obj.Luminosity=lumEncoded.toFixed(2) + " lx";
		}
		
		//Moviment Sensor Accelerometer X
		// 1LSb = 0.061 mg at ±2 g full scale. 
		var movAccelerationXEncoded=(bytes[2]<<8)|(bytes[3]);
		if(movAccelerationXEncoded > 0x8000){ // complemento de 2
		  movAccelerationXEncoded = movAccelerationXEncoded - 65536
		}
		var movAccelerationXDecoded = (movAccelerationXEncoded.toFixed(2) * 0.061)/100;
		obj.MovimAccelerationX=movAccelerationXDecoded.toFixed(2) + " m/s²";

		//Moviment Sensor Accelerometer Y
		// 1LSb = 0.061 mg at ±2 g full scale. 
		var movAccelerationYEncoded=(bytes[4]<<8)|(bytes[5]);
		if(movAccelerationYEncoded > 0x8000){ // complemento de 2
		  movAccelerationYEncoded = movAccelerationYEncoded - 65536;
		}
		var movAccelerationYDecoded = (movAccelerationYEncoded.toFixed(2) * 0.061)/100;
		obj.MovimAccelerationY=movAccelerationYDecoded.toFixed(2) + " m/s²";

		//Moviment Sensor Accelerometer Z
		// 1LSb = 0.061 mg at ±2 g full scale. 
		var movAccelerationZEncoded=(bytes[6]<<8)|(bytes[7]);
		if(movAccelerationZEncoded > 0x8000){ // complemento de 2
		  movAccelerationZEncoded = movAccelerationZEncoded - 65536; 
		}
		var movAccelerationZDecoded = (movAccelerationZEncoded.toFixed(2) * 0.061)/100;
		obj.MovimAccelerationZ=movAccelerationZDecoded.toFixed(2) + " m/s²";
	break;
	
	case 3: 
		// downlink
		//Battery/ Mede a faixa de 3.6V a 3.3V de maneira acurada
		batEncoded=(bytes[0]<<8)|(bytes[1]);
		batDecoded= (batEncoded/1000) * (78/56);
			
		obj.Battery=batDecoded.toFixed(2) + " V";
	
		//Temperatura [ 1 byte ] Sensor
		tempEncoded=(bytes[2]);

		if(tempEncoded > 0x80){ // complemento de 2
		  axtempDecoded = tempEncoded - 256
		}else {
		  axtempDecoded = tempEncoded;
		}
		tempDecoded = 25.00 + axtempDecoded;  
		obj.Temperatura=(parseFloat(tempDecoded)).toFixed(1) + " °C";
		//Luminosity Sensor
		lumEncoded=(bytes[3]);
		if (lumEncoded >= 255) {
		  		obj.Luminosity= "Acima de 255 lx";
		} else {
  		    obj.Luminosity=lumEncoded.toFixed(2) + " lx";
		}
		
		//Moviment Sensor Accelerometer X
		// 1LSb = 0.061 mg at ±2 g full scale. 
		var movAccelerationXEncoded=(bytes[4]<<8)|(bytes[5]);
		if(movAccelerationXEncoded > 0x8000){ // complemento de 2
		  movAccelerationXEncoded = movAccelerationXEncoded - 65536
		}
		var movAccelerationXDecoded = (movAccelerationXEncoded.toFixed(2) * 0.061)/100;
		obj.MovimAccelerationX=movAccelerationXDecoded.toFixed(2) + " m/s²";

		//Moviment Sensor Accelerometer Y
		// 1LSb = 0.061 mg at ±2 g full scale. 
		var movAccelerationYEncoded=(bytes[6]<<8)|(bytes[7]);
		if(movAccelerationYEncoded > 0x8000){ // complemento de 2
		  movAccelerationYEncoded = movAccelerationYEncoded - 65536;
		}
		var movAccelerationYDecoded = (movAccelerationYEncoded.toFixed(2) * 0.061)/100;
		obj.MovimAccelerationY=movAccelerationYDecoded.toFixed(2) + " m/s²";

		//Moviment Sensor Accelerometer Z
		// 1LSb = 0.061 mg at ±2 g full scale. 
		var movAccelerationZEncoded=(bytes[8]<<8)|(bytes[9]);
		if(movAccelerationZEncoded > 0x8000){ // complemento de 2
		  movAccelerationZEncoded = movAccelerationZEncoded - 65536; 
		}
		var movAccelerationZDecoded = (movAccelerationZEncoded.toFixed(2) * 0.061)/100;
		obj.MovimAccelerationZ=movAccelerationZDecoded.toFixed(2) + " m/s²";
	break;
	
	case 5: 
	// downlink hardware
    var nibble1 = ((bytes[0] & 0xF0) >> 4);
    if(nibble1 != 0x0){
      st_n1 = nibble1 + ".";
    } else {
      st_n1 = "";
    }
    var nibble2 = ((bytes[0] & 0x0F));
    if(nibble1 == 0x0 && nibble2 == 0x0){
      st_n2 = ""
    } else {
      st_n2 = nibble2 + ".";
    }
    var nibble3 = ((bytes[1] & 0xF0) >> 4);
    var nibble4 = ((bytes[1] & 0x0F));
    obj.versaoHW ="Versão: " +st_n1 + st_n2 + nibble3 + "." +nibble4;
    nibble1 = ((bytes[2] & 0xF0) >> 4);
    if(nibble1 != 0x0){
      st_n1 = nibble1 + ".";
    } else {
      st_n1 = "";
    }
    nibble2 = ((bytes[2] & 0x0F));
        if(nibble1 == 0x0 && nibble2 == 0x0){
      st_n2 = ""
    } else {
      st_n2 = nibble2 + ".";
    }
    nibble3 = ((bytes[3] & 0xF0) >> 4);
    nibble4 = ((bytes[3] & 0x0F));
    obj.versaoSW = "Versão: " + st_n1 + st_n2 +nibble3 + "." +nibble4;
	break;
	case 6: 
	// downlink software
	var mix_byte_msb = ((bytes[1] & 0xF0) >> 4); 
	var mix_byte_lsb = (bytes[1] & 0x0F);
	var keep_alive = (bytes[0]<<4) | mix_byte_msb;
	keep_alive = keep_alive * 30;
	var keep_alive_st = seconds_to_DDHHMMSS(keep_alive);
	
	var warn_period = ( mix_byte_lsb << 8 )| bytes[2];
	warn_period = warn_period * 5;
	var warn_period_st = seconds_to_DDHHMMSS(warn_period);	
	
	mix_byte_msb = ((bytes[4] & 0xF0) >> 4); 
	mix_byte_lsb = (bytes[4] & 0x0F);
	var warn_tx_timeout = (bytes[3]<<4) | mix_byte_msb;
	warn_tx_timeout = warn_tx_timeout * 5;
	var warn_tx_timeout_st = seconds_to_DDHHMMSS(warn_tx_timeout);	
	
	var bat_ths = ( mix_byte_lsb << 8 )| bytes[5];
	bat_ths = bat_ths / 100;
	
	var lux_ths=(bytes[7]<<8)|(bytes[8]);
  lux_ths = lux_ths & 0x0FFF

  mix_byte_msb = (bytes[6] & 0xF0) >> 4;
  mix_byte_lsb = (bytes[6] & 0x0F);
  var det_ang = (mix_byte_msb & 0xC) >> 2
  
  var queda_livre = (mix_byte_msb & 0x3);
  mix_byte_msb = ((mix_byte_lsb & 0x8) >> 3);
  queda_livre = ((queda_livre << 1 ) | mix_byte_msb);
  mix_byte_msb = (bytes[7] & 0xF0) >> 4;
  var dur_time = ((mix_byte_lsb & 0x07) << 4 ) | mix_byte_msb;
  dur_time = dur_time * 0.625;	

  var st_detecao_angular = "limiar de 80°";
  switch (det_ang) {
    case 0:
      st_detecao_angular = "limiar de 80°";
      break;
    case 1:
      st_detecao_angular = "limiar de 70°";
      break;
    case 2:
      st_detecao_angular = "limiar de 60°"
      break;
    case 3:
      st_detecao_angular = "limiar de 50°";
      break;
    default:
      st_detecao_angular = "limiar de 80°"
      break;
	
  }
    var st_queda_livre = "156 mg";
  switch (queda_livre) {
	case 0:
      st_queda_livre = "156 mg";
    break;
    case 1:
      st_queda_livre = "219 mg";
      break;
    case 2:
      st_queda_livre = "250 mg";
      break;
    case 3:
      st_queda_livre = "312 mg";
      break;
    case 4:
      st_queda_livre = "344 mg";
      break;
    case 5:
      st_queda_livre = "406 mg";
      break;
	case 6:
      st_queda_livre = "469 mg";
      break;
	case 7:
      st_queda_livre = "500 mg";
    break;
	default:
	  st_queda_livre = "156 mg";
	break;
	
  }
	obj.Keep_Alive_Timer = keep_alive_st;
	obj.Warn_TX_Timer = warn_tx_timeout_st;
	obj.Warn_Period_Timer = warn_period_st;
	obj.Battery_threshold=bat_ths.toFixed(2) + " Volts";
	obj.Lux_threshold=lux_ths.toFixed(0) + " lux";
	obj.Mov_Angular_threshold=st_detecao_angular;
	obj.Mov_QuedaLivre_threshold=st_queda_livre;
	obj.Mov_QuedaLivre_duracao =dur_time.toFixed(3) + " segundos";
	break;
	
	case 9:
	    var erro_downlink
	    flag_erro = bytes[0]
	    if((flag_erro & 0x01) == 0x01){
	      erro_downlink = "KeepAlive deve ser maior que Warn Period|";
	    } else {
	      erro_downlink = ""
	    }

	    if((flag_erro & 0x08) == 0x08){
        erro_downlink = erro_downlink.concat(" Comando inexistente ou porta errada |");    
	    } else {
	      erro_downlink = erro_downlink.concat("")
	    }
	    if((flag_erro & 0x04) == 0x04){
	      erro_downlink = erro_downlink.concat(" Keep Alive deve ser maior que WarnTx |");
	    } else {
	      erro_downlink = erro_downlink.concat("")
	    }
	    if((flag_erro & 0x02) == 0x02){
	      erro_downlink = erro_downlink.concat(" Warn Period deve ser maior que warnTx |");
	    } else {
	      erro_downlink = erro_downlink.concat("")
	    }
	    if((flag_erro & 0x10) == 0x10){
	      erro_downlink = erro_downlink.concat(" Limiar de angulo não deve ultrapassar 4 |");
	    } else {
	      erro_downlink = erro_downlink.concat("")
	    }
	    if((flag_erro & 0x20) == 0x20){
	      erro_downlink = erro_downlink.concat(" Limiar de queda livre não deve ultrapassar 8 |");
	    } else {
	      erro_downlink = erro_downlink.concat("")
	    }
	    if((flag_erro & 0x40) == 0x40){
	      erro_downlink = erro_downlink.concat(" Limiar de duracao de queda livre não deve ultrapassar 0x40 |");
	    } else {
	      erro_downlink = erro_downlink.concat("")
	    }
	    if((flag_erro & 0x80) == 0x80){
	      erro_downlink = erro_downlink.concat(" Warn Period deve ser menor que Keep Alive |");
	    } else {
	      erro_downlink = erro_downlink.concat("")
	    }
      
      obj.downlink_failed = String(erro_downlink);

	
	
	break;
	  
	
	default:
    //Battery/ Mede a faixa de 3.6V a 3.3V de maneira acurada e precisa
    var batEncoded=(bytes[0]<<8)|(bytes[1]);
    var batDecoded= (batEncoded/1000) * (78/56)
    
    obj.Battery=batDecoded.toFixed(2) + " V";
    
  
    //Temperatura Sensor
    var tempEncoded=(bytes[2]<<8)|(bytes[3]);
    var axtempDecoded;
    
    if(tempEncoded > 0x8000){ // complemento de 2
      axtempDecoded = tempEncoded - 65536
    }else {
      axtempDecoded = tempEncoded;
    }
    var tempDecoded = 25.00 + (axtempDecoded/256);  
    obj.Temperatura=(parseFloat(tempDecoded)).toFixed(2) + " °C";
    //Luminosity Sensor
    var lumEncoded=(bytes[4]<<8)|(bytes[5]);
    obj.Luminosity=lumEncoded.toFixed(2) + " lx";
    
    //Moviment Sensor Accelerometer X
    // 1LSb = 0.061 mg at ±2 g full scale. 
    var movAccelerationXEncoded=(bytes[6]<<8)|(bytes[7]);
    if(movAccelerationXEncoded > 0x8000){ // complemento de 2
      movAccelerationXEncoded = movAccelerationXEncoded - 65536
    }
    var movAccelerationXDecoded = (movAccelerationXEncoded.toFixed(2) * 0.061)/100;
    obj.MovimAccelerationX=movAccelerationXDecoded.toFixed(2) + " m/s²";
    
    //Moviment Sensor Accelerometer Y
    // 1LSb = 0.061 mg at ±2 g full scale. 
    var movAccelerationYEncoded=(bytes[8]<<8)|(bytes[9]);
    if(movAccelerationYEncoded > 0x8000){ // complemento de 2
      movAccelerationYEncoded = movAccelerationYEncoded - 65536;
    }
    var movAccelerationYDecoded = (movAccelerationYEncoded.toFixed(2) * 0.061)/100;
    obj.MovimAccelerationY=movAccelerationYDecoded.toFixed(2) + " m/s²";
    
    //Moviment Sensor Accelerometer Z
    // 1LSb = 0.061 mg at ±2 g full scale. 
    var movAccelerationZEncoded=(bytes[10]<<8)|(bytes[11]);
    if(movAccelerationZEncoded > 0x8000){ // complemento de 2
      movAccelerationZEncoded = movAccelerationZEncoded - 65536; 
    }
    var movAccelerationZDecoded = (movAccelerationZEncoded.toFixed(2) * 0.061)/100;
    obj.MovimAccelerationZ=movAccelerationZDecoded.toFixed(2) + " m/s²";
    
    //flag de erro 
    var flag_erro = bytes[12];
    if((flag_erro & 0x80) == 0x80) {
    	erro_lum = "ALERTA";
    } else {
    	erro_lum = "OK";
    }
    if ((flag_erro & 0x40) == 0x40) {
    	erro_mov = "ALERTA";
    } else {
    	erro_mov = "OK";
    }
    if((flag_erro & 0x20) == 0x20){
    	erro_bat = "ALERTA";
    } else {
    	erro_bat = "OK";
    }
    if ((flag_erro & 0x04) == 0x04) {
    	erro_sensor = "ALERTA";
    } else {
    	erro_sensor = "OK";
    }
    
    obj.statusLuminosidade = erro_lum;
    obj.statusMovimento = erro_mov;
    obj.statusBateria = erro_bat;  
    obj.statusSensor = erro_sensor;
	break;
	
}


return obj;

}

function seconds_to_DDHHMMSS(segundos) {
	
	var seconds = parseInt(segundos, 10);

	var days = Math.floor(seconds / (3600*24));
	seconds  -= days*3600*24;
	var hrs   = Math.floor(seconds / 3600);
	seconds  -= hrs*3600;
	var mnts = Math.floor(seconds / 60);
	seconds  -= mnts*60;
	
	var dDisplay = days > 0 ? days + (days == 1 ? " dia" : " dias") : "";
	dDisplay = days > 0 ? dDisplay + ((hrs == 0 && mnts == 0 && seconds == 0)? " ":", " ):"" 
	var hDisplay = hrs > 0 ? hrs + (hrs == 1 ? " hora" : " horas") : "";
	hDisplay = hrs > 0 ? hDisplay + ((mnts == 0 && seconds == 0)? " ":", " ):"" 
	var mDisplay = mnts > 0 ? mnts + (mnts == 1 ? " minuto" : " minutos") : "";
	mDisplay = mnts > 0 ? mDisplay + ((seconds == 0)? "":", " ):"" 
	var sDisplay = seconds > 0 ? seconds + (seconds == 1 ? " segundo" : " segundos") : "";

	var st_final = ": " + dDisplay + hDisplay + mDisplay + sDisplay;
	return st_final;
	
}