varying vec2 UV;
uniform sampler2D textureSampler;
        void main(void){
           gl_FragColor = texture2D(textureSampler, UV.xy);
        }
