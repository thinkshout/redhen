<?php

/**
 * @file
 * Contains \Drupal\redhen_contact\RedhenContactListBuilder.
 */

namespace Drupal\redhen_contact;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;
use Drupal\Core\Routing\LinkGeneratorTrait;
use Drupal\Core\Url;

/**
 * Defines a class to build a listing of Contact entities.
 *
 * @ingroup redhen_contact
 */
class RedhenContactListBuilder extends EntityListBuilder {
  use LinkGeneratorTrait;
  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['id'] = $this->t('Contact ID');
    $header['name'] = $this->t('Name');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /* @var $entity \Drupal\redhen_contact\Entity\RedhenContact */
    $row['id'] = $entity->id();
    $row['name'] = $this->l(
      $entity->label(),
      new Url(
        'entity.redhen_contact.edit_form', array(
          'redhen_contact' => $entity->id(),
        )
      )
    );
    return $row + parent::buildRow($entity);
  }

}
