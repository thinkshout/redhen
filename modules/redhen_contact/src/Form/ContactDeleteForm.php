<?php

namespace Drupal\redhen_contact\Form;

use Drupal\Core\Entity\ContentEntityDeleteForm;
use Drupal\redhen_contact\Entity\ContactType;

/**
 * Provides a form for deleting Contact entities.
 *
 * @ingroup redhen_contact
 */
class ContactDeleteForm extends ContentEntityDeleteForm {

  /**
   * {@inheritdoc}
   */
  public function getQuestion() {
    return $this->t('Are you sure you want to delete the @contact-type %name?', [
      '@contact-type' => ContactType::load($this->entity->bundle())->label(),
      '%name' => $this->entity->label()
    ]);
  }

}
