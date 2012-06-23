# RedHen Membership

## Description

The RedHen Membership module provides a framework for managing individual (Contact) and organizational memberships. Optionally, you could assign Drupal user roles to Drupal users associated with active individual or organizational memberships. Memberships are a custom entity type/bundle. Therefore, memberships can be extended with additional fields.

## Configuration

* First, decide if you want to associate Drupal user accounts with RedHen Contacts. This is not a requirement for managing memberships. However, if you want to apply Drupal user roles to user accounts associated with RedHen Contacts, you need to enable this option at: Configuration > RedHen (http://configuration/admin/config/redhen).
* Second, decide if you want RedHen to manage membership activation/expiration dates programmatically at: Configuration > RedHen (http://configuration/admin/config/redhen)
* Then, create and configure one or more membership bundles at: Structure > RedHen > Membership Types (http://yoursite.com/admin/structure/redhen/membership_types).
* Optionally, you can select a Drupal user role to be assigned to user accounts associated with Contacts entities that are associated with active individual or organizational memberships.
* *Note this cascade.* If an organization has an active membership that is configured to apply a Drupal user role. All Drupal user accounts associated with Contact entities that are, in turn, associated with this organization will receive the Drupal user role.

## Usage

* When looking at a Contact or Organization entity, go to the "Memberships" tab.
* Click "Add membership." If more than one membership bundle is available, choose the type of membership you want to create.
* Optionally, enter the start/end dates for the membership.
* Choose the "state" of the membership. ("Active" memberships will apply the associated Drupal user role(s).)