using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace CanStellarBack.Models
{
    public class Telemetry
    {

        public int TeamId { get; set; }

        public string Raw { get; set; }

        public int WorkingTime { get; set; }

        public int NumberOfPackages { get; set; }

        public double CurrentVoltage { get; set; }

        public int Altitude { get; set; }

        public int Speed { get; set; }

        public double Latitude { get; set; }

        public double Longitude { get; set; }

        public DateTime TimeSinceSeperation { get; set; }

        public int DurationOfVideo { get; set; }
    }
}
