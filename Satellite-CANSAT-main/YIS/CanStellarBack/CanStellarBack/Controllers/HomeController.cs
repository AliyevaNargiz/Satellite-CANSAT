using CanStellarBack.Hubs;
using CanStellarBack.Models;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.SignalR;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

namespace CanStellarBack.Controllers
{
    public class HomeController : Controller
    {
        private readonly IHubContext<TelemetryHub> _hubContext;

        public HomeController(IHubContext<TelemetryHub> hubContext)
        {
            _hubContext = hubContext;
        }

        public async Task<IActionResult> IndexAsync()
        {
            using (var reader = new StreamReader("./data/data.csv"))
            {
                    List<Telemetry> telemetries = new List<Telemetry>();

                while (!reader.EndOfStream)
                {
                    var line = reader.ReadLine();
                    var telemetry = new Telemetry();
                    var values = line.Split(',');
                    var raw = "";

                    for (int j = 0; j < values.Length; j++)
                    {
                        raw += "<" + values[j] + ">,";
                    }
                    



                    telemetry.Raw = raw;
                    telemetry.TeamId = int.Parse(values[0]);
                    telemetry.WorkingTime = int.Parse(values[1]);
                    telemetry.NumberOfPackages = int.Parse(values[2]);
                    telemetry.CurrentVoltage = double.Parse(values[3]);
                    telemetry.Altitude = int.Parse(values[4]);
                    telemetry.Speed = int.Parse(values[5]);
                    telemetry.Latitude = double.Parse(values[6]);
                    telemetry.Longitude = double.Parse(values[7]);
                    telemetry.TimeSinceSeperation = DateTime.Parse(values[8]);
                    telemetry.DurationOfVideo = int.Parse(values[9]);
                    

                telemetries.Add(telemetry);
            }

               telemetries.Reverse();
                await _hubContext.Clients.All.SendAsync("ReceiveTelemetryData", telemetries);

                return View(telemetries);
        }
    }


    }


}