CREATE TABLE [dbo].[O_DATA2] (
    [label]      NVARCHAR (20) NOT NULL,
    [port]       INT           NOT NULL,
    [onoff]      INT           NOT NULL,
    [polarity]   INT           NOT NULL,
    [type]       INT           NOT NULL,
    [time_delay] INT           NOT NULL,
    [time_left]  INT           NOT NULL,
    [pulse_time] INT           NOT NULL,
    [reset]      INT           NOT NULL,
    CONSTRAINT [PK_O_DATA2] PRIMARY KEY CLUSTERED ([port] ASC)
);