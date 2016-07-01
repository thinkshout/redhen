<?php

namespace Drupal\redhen_contact\Plugin\Validation\Constraint;

use Symfony\Component\Validator\Constraint;

/**
 * Checks if a Contact's email address is unique on the site.
 *
 * @Constraint(
 *   id = "ContactEmailUnique",
 *   label = @Translation("Contact email unique", context = "Validation")
 * )
 */
class ContactEmailUnique extends Constraint {

  public $message = 'The email address %value is already taken.';

}
