# RedHen CRM Donation

## About RedHen Donation

RedHen Donation allows for a RedHen Donation field to be attached to an entity, which attach a form to that entity for processing donations.
It integrates with Drupal Commerce to handle order processing and payment handling.
It relies on Commerce Card on File and Commere Recurring to process recurring donations.

## Notes

* Donation products should have an amount of 0.
* Recurring donations requires the patchs found at:
  * https://drupal.org/node/2273443
  * https://drupal.org/node/2263371


## Usage

* Enable RedHen Donation
* Create products with a zero price
* Create a Donation Type
* Atach a RedHen Donation field to an entity
* Configure the donation settings for that entity