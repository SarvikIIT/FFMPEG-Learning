#include<stdio.h>
#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
#include<libswscale/swscale.h>
int main(int argc,char* argv[]){
    //Variable Declaration

    const char* video =argv[1];
    const char* result =argv[2];

    int start=2*AV_TIME_BASE;
    int crop_time= 4*AV_TIME_BASE;
    int final_time=start+crop_time;
    int status;
    int True=1;

    AVFormatContext* input_context=NULL;
    AVFormatContext* output_context=NULL;
    AVPacket data; //data of videos is taken in form of packets in FFMPEG

    //Initialize any network dependencies
    avformat_network_init();

    //Opening file and taking input stream

    if(status=(avformat_open_input(&input_context,video,NULL,NULL))<0){
        printf("Cant open the file: %d\n",status);
        return -1;
    }

    if(status=(avformat_find_stream_info(input_context,NULL))<0){
        printf("Cant get the stream info: %d\n",status);
        return -1;
    }

    //Create output context

    avformat_alloc_output_context2(&output_context,NULL,NULL,result);

    if(!output_context){
        printf("Can't create the output context\n");
        return -1;
    }

    //Copy Input Stream to Output Stream

    for(int i=0;i<input_context->nb_streams;++i){
        AVStream* input_stream=input_context->streams[i];
        AVStream* output_stream=avformat_new_stream(output_context,NULL);
    

    if(!output_stream){
        printf("Couldn't create output streams\n");
        return -1;
    }
    
    if(status=(avcodec_parameters_copy(output_stream->codecpar,input_stream->codecpar))<0){
        printf("Couldn't copy the codec parameters: %d\n",status);
        return -1;
    }

        output_stream->codecpar->codec_tag=0;
    }

    //Open the result file

    if(!(output_context->oformat->flags & AVFMT_NOFILE)){
        if(status=(avio_open(&output_context->pb,result,AVIO_FLAG_WRITE))<0){
            printf("The output file couldnt be read: %d\n",status);
            return -1;
        }
    }

    //Write header for the output file

    if(status=(avformat_write_header(output_context,NULL))<0){
        printf("Couldn't write the header in the output file: %d\n",status);
        return -1;
    }

    //Going to the point of trimming

    if(status=(av_seek_frame(input_context,-1,start,AVSEEK_FLAG_BACKWARD))<0){
        printf("Couldn't access that time frame: %d\n",status);
        return -1;
    }

    while(True){

        if(av_read_frame(input_context,&data)<0) break;

        AVStream* input_stream=input_context->streams[data.stream_index];
        AVStream* output_stream=output_context->streams[data.stream_index];

        //Convert PTS/DTS to global timestamps

        int pts_time=av_rescale_q(data.pts,input_stream->time_base,AV_TIME_BASE_Q);

        if(pts_time<start){
            av_packet_unref(&data); //Saare packets ko before and after the time frame of crop ko free karte jao
            continue;
        }

        if(pts_time>final_time){
            av_packet_unref(&data);
            break;
        }

        //Rescale wrt the input and output time base

        data.pts = av_rescale_q_rnd(data.pts, input_stream->time_base, output_stream->time_base, AV_ROUND_NEAR_INF);
        data.dts = av_rescale_q_rnd(data.dts, input_stream->time_base, output_stream->time_base, AV_ROUND_NEAR_INF);
        data.duration=av_rescale_q(data.duration,input_stream->time_base,output_stream->time_base);
        data.pos=-1;

        if(status=(av_interleaved_write_frame(output_context,&data))<0){
            printf("Error muxing the packet: %d \n",status);
            break;
        }

        av_packet_unref(&data); //Free equivalent in FFMPEG

    }
        av_write_trailer(output_context);  

        //Finishing and Cleaning

        avformat_close_input(&input_context);

        if(output_context && !(output_context->oformat->flags & AVFMT_NOFILE)) avio_closep(&output_context->pb);

        avformat_free_context(output_context);

        printf("Code ran successfully!");
        return 0;
}