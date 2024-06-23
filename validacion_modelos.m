%Programa de Identificacion
load('datos_modelado_horno.mat')
plot(ts,us,ts,ys,'linewidth',3),grid
title('Datos de la identificación')
xlabel('Tiempo (s)')
ylabel('Temperatura (C)')

b=20; % Escalon donde se hizo lA IDENTIFICACION 20 40 60 -40 -20 0 
        % los valores negativos son para los escalones descendentes


 switch (b)
     case 20
        %Busca el momento exacto donde se insertó el escalón
         i=1;
        while(us(i)<b)
           i=i+1;
        end
            x1=i;
        while(us(i)==b)
             i=i+1;
        end
            x2=i-1;

        %Recortar datos hasta el origen
        ur=us(x1:x2)';
        yr=ys(x1:x2)';
        
        tr=ts(x1:x2)';
        %transladar los datos al origen
        
        ut=ur-us(1); %us(1) tiene valor 0 aqui
        yt=yr-yr(1);
        
        tt=tr-tr(1);
        
        %Graficar escalon Trasladado
        figure(2)
        plot(tt,ut,tt,yt,'linewidth',3),grid
        title('Datos recortados y transladados')
        xlabel('Tiempo (s)')
        ylabel('Temperatura (C)')
    case 40  
        %Busca el momento exacto donde se insertó el escalón
         i=1;
        while(us(i)<b)
           i=i+1;
        end
            x1=i;
        while(us(i)==b)
             i=i+1;
        end
            x2=i-1;

        %Recortar datos hasta el origen
        ur=us(x1:x2)';
        yr=ys(x1:x2)';
        
        tr=ts(x1:x2)';
        %transladar los datos
        
        ut=ur-20; % se resta 20 porque se ha dado un escalon de 20 no de 40
        yt=yr-yr(1);
        
        tt=tr-tr(1);
        
        %Graficar escalon Trasladado
        figure(2)
        plot(tt,ut,tt,yt,'linewidth',3),grid
        title('Datos recortados y transladados')
        xlabel('Tiempo (s)')
        ylabel('Temperatura (C)')
    case 60 
        %Busca el momento exacto donde se insertó el escalón
         i=1;
        while(us(i)<b)
           i=i+1;
        end
            x1=i;
        while(us(i)==b)
             i=i+1;
        end
            x2=i-1;

        %Recortar datos hasta el origen
        ur=us(x1:x2)';
        yr=ys(x1:x2)';
        
        tr=ts(x1:x2)';
        %transladar los datos
        
        ut=ur-40;
        yt=yr-yr(1);
        
        tt=tr-tr(1);
        
        %Graficar escalon Trasladado
        figure(2)
        plot(tt,ut,tt,yt,'linewidth',3),grid
        title('Datos recortados y transladados')
        xlabel('Tiempo (s)')
        ylabel('Temperatura (C)')
      case -40 
        %Busca el momento exacto donde se insertó el escalón
         i=13628; % iniciamos los datos en u(13628)=60, cambiar si es necesario
         e=40;
        while(us(i)>e)
           i=i+1;
        end
            x1=i;
        while(us(i)==e)
             i=i+1;
        end
            x2=i-1;

        %Recortar datos hasta el origen
        ur=us(x1:x2)';
        yr=ys(x1:x2)';
        
        tr=ts(x1:x2)';
        %transladar los datos
        
        ut=ur-60;
        yt=yr-yr(1);
        
        tt=tr-tr(1);
        
        %Graficar escalon Trasladado
        figure(2)
        plot(tt,ut,tt,yt,'linewidth',3),grid
        title('Datos recortados y transladados')
        xlabel('Tiempo (s)')
        ylabel('Temperatura (C)')
     case -20 
        %Busca el momento exacto donde se insertó el escalón
         i=17849; % iniciamos los datos en u(17849)=40, cambiar si es necesario
         e=20;
        while(us(i)>e)
           i=i+1;
        end
            x1=i;
        while(us(i)==e)
             i=i+1;
        end
            x2=i-1;

        %Recortar datos hasta el origen
        ur=us(x1:x2)';
        yr=ys(x1:x2)';
        
        tr=ts(x1:x2)';
        %transladar los datos
        
        ut=ur-40;
        yt=yr-yr(1);
        
        tt=tr-tr(1);
        
        %Graficar escalon Trasladado
        figure(2)
        plot(tt,ut,tt,yt,'linewidth',3),grid
        title('Datos recortados y transladados')
        xlabel('Tiempo (s)')
        ylabel('Temperatura (C)')
    case 0
        %Busca el momento exacto donde se insertó el escalón
         i=22755;% iniciamos los datos en u(17849)=40,datos verificacion cambiar si es necesario
         e=0;
        while(us(i)>e)
           i=i+1;
        end
            x1=i;
            x2=length(us);

        %Recortar datos hasta el origen
        ur=us(x1:x2)';
        yr=ys(x1:x2)';
        
        tr=ts(x1:x2)';
        %transladar los datos
        
        ut=ur-20;
        yt=yr-yr(1);
        
        tt=tr-tr(1);
        
        %Graficar escalon Trasladado
        figure(2)
        plot(tt,ut,tt,yt,'linewidth',3),grid
        title('Datos recortados y transladados')
        xlabel('Tiempo (s)')
        ylabel('Temperatura (C)')
 end