idf_component_register(
    
                SRCS "main.c" "wifi.c" "mqtt.c" "iterations.c" "ota.c" 
                INCLUDE_DIRS "."    
                REQUIRES esp-tls esp_wifi esp_driver_gpio nvs_flash esp_event espressif__mqtt app_update esp_https_ota
                EMBED_TXTFILES "emqxsl-ca.crt"
                )



# ======= REQUIRES ======= Quando você remove todas as dependências explícitas (deixando sem a palavra-chave REQUIRES), a pasta main ganha acesso automático a uma lista gigante de componentes padrão do framework.No entanto, no momento em que você adiciona qualquer componente no REQUIRES (como o esp_wifi), essa regra automática é desativada. O CMake assume que você quer gerenciar as dependências manualmente e passa a enxergar apenas o que você listou ali. 
