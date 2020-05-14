<?php

namespace Drupal\redhen_org\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Link;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\Request;


/**
 * Class OrgAddController.
 *
 * @package Drupal\redhen_org\Controller
 */
class OrgAddController extends ControllerBase {
  public function __construct(EntityStorageInterface $storage, EntityStorageInterface $type_storage) {
    $this->storage = $storage;
    $this->typeStorage = $type_storage;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    /** @var EntityTypeManagerInterface $entity_type_manager */
    $entity_type_manager = $container->get('entity_type.manager');
    return new static(
      $entity_type_manager->getStorage('redhen_org'),
      $entity_type_manager->getStorage('redhen_org_type')
    );
  }

  /**
   * Displays add links for available bundles/types for entity redhen_org .
   *
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   The current request object.
   *
   * @return array
   *   A render array for a list of the redhen_org bundles/types that can be added or
   *   if there is only one type/bunlde defined for the site, the function returns the add page for that bundle/type.
   */
  public function add(Request $request) {
    $types = $this->typeStorage->loadMultiple();
    if ($types && count($types) == 1) {
      $type = reset($types);
      return $this->addForm($type, $request);
    }
    if (count($types) === 0) {
      return [
        '#markup' => $this->t('You have not created any %bundle types yet. @link to add a new type.', [
          '%bundle' => 'Org',
          '@link' => Link::createFromRoute($this->t('Go to the type creation page'), 'entity.redhen_org_type.add_form'),
        ]),
      ];
    }
    return ['#theme' => 'redhen_org_content_add_list', '#content' => $types];
  }

  /**
   * Presents the creation form for redhen_org entities of given bundle/type.
   *
   * @param \Drupal\Core\Entity\EntityInterface $redhen_org_type
   *   The custom bundle to add.
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   The current request object.
   *
   * @return array
   *   A form array as expected by drupal_render().
   */
  public function addForm(EntityInterface $redhen_org_type, Request $request) {
    $entity = $this->storage->create([
      'type' => $redhen_org_type->id(),
    ]);
    return $this->entityFormBuilder()->getForm($entity);
  }

  /**
   * Provides the page title for this controller.
   *
   * @param \Drupal\Core\Entity\EntityInterface $redhen_org_type
   *   The custom bundle/type being added.
   *
   * @return string
   *   The page title.
   */
  public function getAddFormTitle(EntityInterface $redhen_org_type) {
    return t('Create of bundle @label',
      ['@label' => $redhen_org_type->label()]
    );
  }

}
