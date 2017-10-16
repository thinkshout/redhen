<?php

namespace Drupal\redhen_org\Form;

use Drupal\Core\Entity\ContentEntityDeleteForm;
use Drupal\redhen_org\Entity\OrgType;

/**
 * Provides a form for deleting Org entities.
 *
 * @ingroup redhen_org
 */
class OrgDeleteForm extends ContentEntityDeleteForm {

  /**
   * {@inheritdoc}
   */
  public function getQuestion() {
    return $this->t('Are you sure you want to delete the @org-type %name?', [
      '@org-type' => OrgType::load($this->entity->bundle())->label(),
      '%name' => $this->entity->label()
    ]);
  }

}
