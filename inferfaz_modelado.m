%% Monitoreo de señales en tiempo Real
function varargout=interfaz_modelado(varargin)
parar=false;
ready=true;
auto=false;
manu=false;
fclose('all')
global tiempo salida escalon control

fig(1)=figure('name','Interfaz de la temperatura del horno','menubar','none','position',[200 200 800 700],'color',[0 0.6 0])
movegui(fig(1),'center');
axe(1)=axes('parent',fig(1),'units','pixels','position',[60 380 600 280],'xlim',[0 40],'ylim',[0 100],'xgrid','on','ygrid','on')
axe(2)=axes('parent',fig(1),'units','pixels','position',[60 50 600 280],'xlim',[0 40],'ylim',[0 100],'xgrid','on','ygrid','on')

set(get(axe(1),'XLabel'),'String','Tiempo (Seg)')
set(get(axe(1),'YLabel'),'String','Temperatura (°C)')
set(get(axe(2),'XLabel'),'String','Tiempo (Seg)')
set(get(axe(2),'YLabel'),'String','Control (%)')

lin(1)=line('parent',axe(1),'xdata',[],'ydata',[],'Color','r','LineWidth',2.5);
lin(2)=line('parent',axe(1),'xdata',[],'ydata',[],'Color','k','LineWidth',2);
lin(3)=line('parent',axe(2),'xdata',[],'ydata',[],'Color','r','LineWidth',2.5);

Texto(1)=uicontrol('parent',fig(1),'style','text','string','Puerto','position',[680 630 100 50],'BackgroundColor',[0.9 0.6 0.3],'fontsize',18);
Texto(2)=uicontrol('parent',fig(1),'style','text','string','Setpoint','position',[680 260 100 50],'BackgroundColor',[0.9 0.6 0.3],'fontsize',18);
Texto(3)=uicontrol('parent',fig(1),'style','text','string','Gráfico','position',[680 370 100 50],'BackgroundColor',[0.9 0.6 0.3],'fontsize',18);

bot(1)=uicontrol('parent',fig(1),'style','pushbutton','string','Detener','position',[680 50 100 50],'callback',@stop,'fontsize',11)
bot(2)=uicontrol('parent',fig(1),'style','pushbutton','string','Enviar','position',[680 160 100 50],'callback',@enviar,'fontsize',11)
bot(3)=uicontrol('parent',fig(1),'style','pushbutton','string','Salvar','position',[680 320 100 50],'callback',@salvar,'fontsize',11)
bot(4)=uicontrol('parent',fig(1),'style','pushbutton','string','Iniciar','position',[680 560 100 50],'callback',@iniciar,'fontsize',11)
bot(5)=uicontrol('parent',fig(1),'style','pushbutton','string','Manual','position',[680 480 100 50],'callback',@manual,'fontsize',11)
bot(6)=uicontrol('parent',fig(1),'style','pushbutton','string','Automático','position',[680 430 100 50],'callback',@automatico,'fontsize',11)
txbx(1)=uicontrol('parent',fig(1),'style','tex','string','Temp','position',[680 100 100 50],'fontsize',11)
txbx(2)=uicontrol('parent',fig(1),'style','edit','string','000','position',[680 210 100 50],'fontsize',11)

start=0;
ports = serialportlist;
if isempty(ports)
    ports ='NONE';
    clc
    disp('No se ha encontrado CONEXIÓN con el Dispositivo de Control');
else
    start=1;
end
puerta =ports(1);
popup = uicontrol('parent',fig(1),'Style', 'popup','String', ports,'Position', [680 600 100 50],'fontsize',15,'Callback', @puertas); 

%% Funcion iniciar
function varargout=iniciar(hObject,evendata)
    ready=false;
end
%% Funcion Pare
    function varargout=stop(hObject,evendata)
        parar=true;
        write(arduino,"I"+"300"+"F",'char');
        fclose(arduino);
        delete(arduino);
        clear arduino;
        
    end

%% Funcion enviar
    function varargout=enviar(hObject,evendata)
        deg1=get(txbx(2),'string');
         if auto==true
         deg=["I"+deg1+"F"];
         write(arduino,deg,'char'); 
         end
         if manu==true
          deg=["I"+deg1+"F"];
          write(arduino,deg,'char');
         end
        
        degm=["I"+deg1+"F"];
        write(arduino,degm,'char'); 
       
    end
%% Funcion manual
    function varargout=manual(hObject,evendata)
         man="301";
        write(arduino,"I"+man+"F",'char');
        manu=true;
        auto=false;
      
    end
 
%% Funcion automático
    function varargout=automatico(hObject,evendata)
         aut="302";
        write(arduino,"I"+aut+"F",'char'); 
        auto=true;
        manu=false;
       
    end
 
 %% Funcion Puerto serie
    function varargout=puertas(hObject,evendata)
        
        puerta=popup.String{popup.Value};
    end
%% Funcion Salvar
    function varargout=salvar(hObject,evendata)
        %Renombra variables 
        rs=escalon;
        us=control;
        ys=salida;
        ts=tiempo;
       
        %Grafica datos
        figure
        subplot(2,1,1);
        plot(ts,rs,ts,ys,'linewidth',3),grid
        
        title('Laboratorio de Temperatura')
        xlabel('Tiempo (s)')
        ylabel('Temperatura (C)')

        subplot(2,1,2);
        plot(ts,us,'linewidth',3),grid
        xlabel('Tiempo (s)')
        ylabel('Control (%)')
      
        T=[ts;rs;ys;us];
        filter = {'*.txt';'*.xls'};
        [file,path,indx] = uiputfile(filter);
        fileID = fopen(strcat(path,file),'w');
        fprintf(fileID,'%12s %12s %12s %12s\n','t','r','y','u');
        fprintf(fileID,'%12.2f %12.2f %12.2f %12.2f\n',T);
        fclose(fileID);
        file(end-2:end)='mat';
        save(strcat(path,file),'ts','rs','ys','us')
           
        
    end
    %% funcion Graficar
   % function varargout=grafique(hObject,evendata)
        tiempo=[0];
        salida=[0];
        escalon=[0];
        control=[0];
        deg1="0";
   
        dt=1;
        limx=[0 40];
        limy=[0 100];
        set(axe(1),'xlim',limx,'ylim',limy);
   while(ready)
       pause(1);
   end
  if start        
    %% Configura el Puerto Serial
    
    arduino=serialport(puerta,9600,"StopBits",1,"DataBits",8);
    fopen(arduino);
    % Aquí se usa flushinput() para limpiar el buffer  
    flushinput(arduino);         
     %% Grafico
     k=5; nit =200000;
    while(~parar)
        % Lectura de Datos por Puerto Serial
        % Leo datos del arduino, el tiempo, la temperatura y la señal de control
         data=readline(arduino); 
        % Separo los datos y los convierto en un vector de caracteres
         Cadena = strsplit(data);
        % El primer dato de cadena lo transformo de cadena a un numero (double) y divido entre mil
         time=((str2double(Cadena(1)))/1000);
        %El segundo dato de cadena lo transformo de cadena a un numero (double) y divido entre mil
         temp=(str2double(Cadena(2)))/100;
         tempC=string(temp);
         ctr=str2double(Cadena(3));
         %cargo el valor de temperatura en la caja de texto
         set(txbx(1),'string',tempC);
         %Actualiza las variables del grafico
         tiempo=[tiempo time];
         salida=[salida temp];
         control=[control ctr]; 
         escalon=[escalon str2double(deg1)];
         %Cargo los datos en los axes
         set(lin(1),'xdata',tiempo,'ydata',salida);
         set(lin(2),'xdata',tiempo,'ydata',escalon);
         set(lin(3),'xdata',tiempo,'ydata',control);
         %Se espera un segundo para volver a recoger los datos
         pause(dt);
         % actualizo grafica cuando llega a su limite en tiempo real
         if tiempo(end)>=limx 
            limx=[0 limx(2)+40];
            set(axe(1),'xlim',limx) ;
            set(axe(2),'xlim',limx); 
         end
         %actualizo grafica cuando llega a su limite en tiempo real
         if salida(end)>=limy 
            limy=[0 limy(2)+10];
            set(axe(1),'ylim',limy); 
         end
         %actualizo grafica cuando llega a su limite en tiempo real
         if escalon(end)>=limy 
            limy=[0 escalon(end)+10];
            set(axe(1),'ylim',limy); 
         end
         %Actualizo el contador
         k=k+1;
         if(k==nit)
            parar=true;
         end

     end
        parar=false;
   end
end
