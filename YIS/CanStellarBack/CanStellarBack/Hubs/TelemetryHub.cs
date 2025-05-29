using CanStellarBack.Models;
using Microsoft.AspNet.SignalR.Hubs;
using Microsoft.AspNetCore.SignalR;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace CanStellarBack.Hubs
{
    [HubName("telemetryHub")]
    public class TelemetryHub : Hub
    {
        public async Task SendTelemetryData(List<Telemetry> telemetries)
        {
            await Clients.All.SendAsync("ReceiveTelemetryData", telemetries);
        }


    }
}
